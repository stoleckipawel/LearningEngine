// =============================================================================
// InputCallbackScope.h — Container for Multiple Callback Handles
// =============================================================================
//
// RAII container that manages multiple callback handles. All handles are
// automatically unsubscribed when the scope is destroyed.
//
// USAGE:
//   InputCallbackScope scope;
//   scope.Add(inputSystem.SubscribeKeyPressed(...));
//   scope.Add(inputSystem.SubscribeMouseMove(...));
//   // All callbacks unsubscribed when scope is destroyed
//
// =============================================================================

#pragma once

#include "InputCallbackHandle.h"

#include <vector>

namespace Input
{

// =============================================================================
// InputCallbackScope — Container for Multiple Handles
// =============================================================================

/// RAII container that manages multiple callback handles. All handles are
/// automatically unsubscribed when the scope is destroyed.
///
/// Useful for components that register multiple callbacks and want them all
/// cleaned up together (e.g., CameraController).
class InputCallbackScope
{
public:
	// -------------------------------------------------------------------------
	// Constructors / Destructor
	// -------------------------------------------------------------------------

	/// Default constructor - creates an empty scope.
	InputCallbackScope() noexcept = default;

	/// Destructor - unsubscribes all handles.
	~InputCallbackScope();

	// -------------------------------------------------------------------------
	// Move Semantics (Move-Only)
	// -------------------------------------------------------------------------

	/// Move constructor.
	InputCallbackScope(InputCallbackScope&& other) noexcept;

	/// Move assignment.
	InputCallbackScope& operator=(InputCallbackScope&& other) noexcept;

	// -------------------------------------------------------------------------
	// Deleted Copy Operations
	// -------------------------------------------------------------------------

	InputCallbackScope(const InputCallbackScope&) = delete;
	InputCallbackScope& operator=(const InputCallbackScope&) = delete;

	// -------------------------------------------------------------------------
	// Public Interface
	// -------------------------------------------------------------------------

	/// Adds a handle to this scope. Takes ownership via move.
	void Add(InputCallbackHandle&& handle);

	/// Unsubscribes and removes all handles.
	void Clear() noexcept;

	/// Returns the number of handles in this scope.
	[[nodiscard]] std::size_t Size() const noexcept;

	/// Returns true if the scope has no handles.
	[[nodiscard]] bool IsEmpty() const noexcept;

	/// Reserves capacity for the specified number of handles.
	void Reserve(std::size_t capacity);

private:
	std::vector<InputCallbackHandle> m_handles;
};

}  // namespace Input
