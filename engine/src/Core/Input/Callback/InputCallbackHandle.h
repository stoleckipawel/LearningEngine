// =============================================================================
// InputCallbackHandle.h — RAII Subscription Handle
// =============================================================================
//
// Move-only handle for automatic callback unsubscription. When the handle goes
// out of scope, the associated callback is automatically unregistered.
//
// USAGE:
//   // Subscribe returns a handle - store it to keep subscription alive
//   InputCallbackHandle handle = inputSystem.SubscribeKeyPressed([](const KeyboardEvent& e) {
//       // Handle key press
//   });
//
//   // When handle goes out of scope, callback is automatically unsubscribed
//
//   // Or manually unsubscribe:
//   handle.Unsubscribe();
//
// See also: InputCallbackScope.h for managing multiple handles
//
// =============================================================================

#pragma once

#include <cstdint>
#include <functional>

namespace Input
{

// =============================================================================
// InputCallbackHandle — RAII Subscription Handle
// =============================================================================

/// Move-only handle that automatically unsubscribes a callback when destroyed.
/// The unsubscribe function is provided by the InputSystem at registration time.
class InputCallbackHandle
{
public:
	/// Type alias for the unsubscribe function.
	using UnsubscribeFunc = std::function<void()>;

	// -------------------------------------------------------------------------
	// Constructors / Destructor
	// -------------------------------------------------------------------------

	/// Default constructor - creates an invalid handle.
	InputCallbackHandle() noexcept = default;

	/// Constructs a handle with an ID and unsubscribe function.
	/// @param id Unique callback ID assigned by InputSystem.
	/// @param unsubscribeFunc Function to call when unsubscribing.
	explicit InputCallbackHandle(std::uint64_t id, UnsubscribeFunc unsubscribeFunc) noexcept;

	/// Destructor - automatically unsubscribes if valid.
	~InputCallbackHandle();

	// -------------------------------------------------------------------------
	// Move Semantics (Move-Only)
	// -------------------------------------------------------------------------

	/// Move constructor - transfers ownership.
	InputCallbackHandle(InputCallbackHandle&& other) noexcept;

	/// Move assignment - transfers ownership, unsubscribes previous if valid.
	InputCallbackHandle& operator=(InputCallbackHandle&& other) noexcept;

	// -------------------------------------------------------------------------
	// Deleted Copy Operations
	// -------------------------------------------------------------------------

	InputCallbackHandle(const InputCallbackHandle&) = delete;
	InputCallbackHandle& operator=(const InputCallbackHandle&) = delete;

	// -------------------------------------------------------------------------
	// Public Interface
	// -------------------------------------------------------------------------

	/// Manually unsubscribe the callback. Safe to call multiple times.
	void Unsubscribe() noexcept;

	/// Returns true if this handle has a valid subscription.
	[[nodiscard]] bool IsValid() const noexcept;

	/// Boolean conversion operator for validity checks.
	[[nodiscard]] explicit operator bool() const noexcept;

	/// Returns the unique callback ID (0 if invalid).
	[[nodiscard]] std::uint64_t GetId() const noexcept;

	/// Releases ownership without unsubscribing. Returns the ID.
	/// Use with caution - the callback will remain registered.
	[[nodiscard]] std::uint64_t Release() noexcept;

private:
	std::uint64_t m_id = 0;             ///< Unique callback ID (0 = invalid)
	UnsubscribeFunc m_unsubscribeFunc;  ///< Function to call on unsubscribe
};

}  // namespace Input
