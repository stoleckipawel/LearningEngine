#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <utility>

//------------------------------------------------------------------------------
// Event System
//------------------------------------------------------------------------------
// Multicast event dispatcher for decoupled notification broadcasting.
// Suitable for low-frequency events (settings changes, resize, etc.),
// not intended for per-frame high-frequency hot paths.
//
// Usage:
//   Event<void(int, float)> OnValueChanged;
//   auto handle = OnValueChanged.Add([](int a, float b) { ... });
//   OnValueChanged.Broadcast(42, 3.14f);
//   OnValueChanged.Remove(handle);
//
// Design:
//   - Fixed capacity to avoid runtime heap allocations
//   - Stable handles for safe removal from any context
//   - No RTTI or exceptions
//------------------------------------------------------------------------------

// Opaque handle returned by Add(), used for Remove()
struct EventHandle
{
	std::uint32_t Id = 0;

	bool IsValid() const noexcept { return Id != 0; }
	void Invalidate() noexcept { Id = 0; }

	bool operator==(const EventHandle& Other) const noexcept { return Id == Other.Id; }
	bool operator!=(const EventHandle& Other) const noexcept { return Id != Other.Id; }
};

// Forward declaration
template <typename Signature, std::size_t Capacity = 8> class Event;

//------------------------------------------------------------------------------
// Event<void(Args...), Capacity>
//------------------------------------------------------------------------------
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

	// Add a listener. Returns handle for later removal.
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

	// Remove a listener by handle
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

	// Invoke all registered listeners
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

	// Check if any listeners are registered
	[[nodiscard]] bool IsBound() const noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_Entries[i].Handle.IsValid())
				return true;
		}
		return false;
	}

	// Remove all listeners
	void Clear() noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			m_Entries[i].Handle.Invalidate();
			m_Entries[i].Callback = nullptr;
		}
	}

	// Debug: number of active subscriptions
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
	struct Entry
	{
		EventHandle Handle;
		CallbackType Callback;
	};

	std::array<Entry, Capacity> m_Entries{};
	std::uint32_t m_NextId = 0;
};

//------------------------------------------------------------------------------
// ScopedEventHandle
//------------------------------------------------------------------------------
// RAII guard that automatically removes subscription on destruction.
// Stores a type-erased cleanup function to avoid template parameter coupling.
//------------------------------------------------------------------------------
class ScopedEventHandle
{
  public:
	ScopedEventHandle() noexcept = default;

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

	void Reset() noexcept
	{
		if (m_Handle.IsValid() && m_RemoveFn)
		{
			m_RemoveFn();
		}
		m_Handle.Invalidate();
		m_RemoveFn = nullptr;
	}

	[[nodiscard]] bool IsValid() const noexcept { return m_Handle.IsValid(); }
	[[nodiscard]] EventHandle GetHandle() const noexcept { return m_Handle; }

  private:
	EventHandle m_Handle;
	std::function<void()> m_RemoveFn;
};
