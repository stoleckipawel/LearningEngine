// ============================================================================
// Event.h
// ----------------------------------------------------------------------------
// Multicast event dispatcher for decoupled notification broadcasting.
//
// USAGE:
//   Event<void(int, float)> OnValueChanged;
//   auto handle = OnValueChanged.Add([](int a, float b) { ... });
//   OnValueChanged.Broadcast(42, 3.14f);
//   OnValueChanged.Remove(handle);
//
//   // RAII auto-unsubscribe:
//   ScopedEventHandle scoped(OnValueChanged, handle);
//
// DESIGN:
//   - Fixed capacity (template parameter) avoids runtime heap allocations
//   - Stable handles for safe removal from any context
//   - No RTTI or exceptions
//   - ScopedEventHandle provides RAII unsubscription
//
// INTENDED USE:
//   Suitable for low-frequency events (settings changes, resize, etc.),
//   not intended for per-frame high-frequency hot paths.
//
// NOTES:
//   - Default capacity is 8 listeners per event
//   - Asserts on capacity overflow in debug builds
// ============================================================================

#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <utility>

// ============================================================================
// EventHandle
// ============================================================================

/// Opaque handle returned by Event::Add(), used for Event::Remove().
struct EventHandle
{
	std::uint32_t Id = 0;  ///< Unique identifier (0 = invalid)

	/// Returns true if handle points to a valid subscription.
	bool IsValid() const noexcept { return Id != 0; }

	/// Marks this handle as invalid.
	void Invalidate() noexcept { Id = 0; }

	bool operator==(const EventHandle& Other) const noexcept { return Id == Other.Id; }
	bool operator!=(const EventHandle& Other) const noexcept { return Id != Other.Id; }
};

// Forward declaration
template <typename Signature, std::size_t Capacity = 8> class Event;

// ============================================================================
// Event<void(Args...), Capacity>
// ============================================================================

/// Multicast event with fixed-capacity listener storage.
template <typename... Args, std::size_t Capacity> class Event<void(Args...), Capacity>
{
  public:
	using CallbackType = std::function<void(Args...)>;

	Event() noexcept = default;
	~Event() = default;

	// Non-copyable, non-movable (prevents handle invalidation)
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;
	Event(Event&&) = delete;
	Event& operator=(Event&&) = delete;

	// ========================================================================
	// Subscription Management
	// ========================================================================

	/// Adds a listener callback. Returns handle for later removal.
	/// @return Valid handle, or invalid handle if capacity exceeded.
	[[nodiscard]] EventHandle Add(CallbackType Callback) noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (!m_Entries[i].Handle.IsValid())
			{
				m_Entries[i].Handle.Id = ++m_NextId;
				m_Entries[i].Callback = std::move(Callback);
				return m_Entries[i].Handle;
			}
		}

		// Capacity exceeded
		assert(false && "Event capacity exceeded. Increase template Capacity parameter.");
		return EventHandle{};
	}

	/// Removes a listener by handle. No-op if handle is invalid.
	void Remove(EventHandle Handle) noexcept
	{
		if (!Handle.IsValid())
			return;

		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_Entries[i].Handle == Handle)
			{
				m_Entries[i].Handle.Invalidate();
				m_Entries[i].Callback = nullptr;
				return;
			}
		}
	}

	/// Removes all listeners.
	void Clear() noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			m_Entries[i].Handle.Invalidate();
			m_Entries[i].Callback = nullptr;
		}
	}

	// ========================================================================
	// Broadcasting
	// ========================================================================

	/// Invokes all registered listeners with the given arguments.
	void Broadcast(Args... InArgs) const
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_Entries[i].Handle.IsValid() && m_Entries[i].Callback)
			{
				m_Entries[i].Callback(InArgs...);
			}
		}
	}

	// ========================================================================
	// Queries
	// ========================================================================

	/// Returns true if any listeners are registered.
	[[nodiscard]] bool IsBound() const noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_Entries[i].Handle.IsValid())
				return true;
		}
		return false;
	}

	/// Returns the number of active subscriptions.
	[[nodiscard]] std::size_t GetBoundCount() const noexcept
	{
		std::size_t Count = 0;
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_Entries[i].Handle.IsValid())
				++Count;
		}
		return Count;
	}

  private:
	// ------------------------------------------------------------------------
	// Internal Storage
	// ------------------------------------------------------------------------

	struct Entry
	{
		EventHandle Handle;      ///< Subscription identifier
		CallbackType Callback;   ///< Listener function
	};

	std::array<Entry, Capacity> m_Entries{};  ///< Fixed-size listener storage
	std::uint32_t m_NextId = 0;               ///< Counter for unique handle IDs
};

// ============================================================================
// ScopedEventHandle
// ============================================================================

/// RAII guard that automatically removes subscription on destruction.
/// Stores a type-erased cleanup function to avoid template parameter coupling.
class ScopedEventHandle
{
  public:
	ScopedEventHandle() noexcept = default;

	/// Constructs a scoped handle that will unsubscribe on destruction.
	template <typename Signature, std::size_t Capacity>
	ScopedEventHandle(Event<Signature, Capacity>& InEvent, EventHandle InHandle) noexcept :
	    m_Handle(InHandle),
	    m_RemoveFn(
	        [&InEvent, InHandle]()
	        {
		        InEvent.Remove(InHandle);
	        })
	{
	}

	~ScopedEventHandle() { Reset(); }

	// Move-only
	ScopedEventHandle(ScopedEventHandle&& Other) noexcept : m_Handle(Other.m_Handle), m_RemoveFn(std::move(Other.m_RemoveFn))
	{
		Other.m_Handle.Invalidate();
		Other.m_RemoveFn = nullptr;
	}

	ScopedEventHandle& operator=(ScopedEventHandle&& Other) noexcept
	{
		if (this != &Other)
		{
			Reset();
			m_Handle = Other.m_Handle;
			m_RemoveFn = std::move(Other.m_RemoveFn);
			Other.m_Handle.Invalidate();
			Other.m_RemoveFn = nullptr;
		}
		return *this;
	}

	ScopedEventHandle(const ScopedEventHandle&) = delete;
	ScopedEventHandle& operator=(const ScopedEventHandle&) = delete;

	/// Unsubscribes and invalidates this handle.
	void Reset() noexcept
	{
		if (m_Handle.IsValid() && m_RemoveFn)
		{
			m_RemoveFn();
		}
		m_Handle.Invalidate();
		m_RemoveFn = nullptr;
	}

	/// Returns true if this scoped handle is active.
	[[nodiscard]] bool IsValid() const noexcept { return m_Handle.IsValid(); }

	/// Returns the underlying event handle.
	[[nodiscard]] EventHandle GetHandle() const noexcept { return m_Handle; }

  private:
	EventHandle m_Handle;              ///< Wrapped subscription handle
	std::function<void()> m_RemoveFn;  ///< Type-erased unsubscribe function
};
