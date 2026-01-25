// =============================================================================
// KeyboardEvent.h — Keyboard Input Event
// =============================================================================
//
// Event fired when a keyboard key is pressed or released.
//
// =============================================================================

#pragma once

#include "../Keyboard/Key.h"
#include "../Keyboard/ModifierFlags.h"
#include "../State/ButtonState.h"
#include "../Device/InputDevice.h"

namespace Input
{

// =============================================================================
// KeyboardEvent
// =============================================================================

/// Event data for keyboard key press/release.
struct KeyboardEvent
{
	Key KeyCode = Key::Unknown;              ///< Which key was pressed/released
	ButtonState State = ButtonState::Up;     ///< Press or release
	ModifierFlags Modifiers = ModifierFlags::None;  ///< Active modifiers (Ctrl, Shift, etc.)
	bool bIsRepeat = false;                  ///< True if this is an OS auto-repeat

	/// Returns the device type (always Keyboard).
	[[nodiscard]] constexpr InputDevice GetDevice() const noexcept
	{
		return InputDevice::Keyboard;
	}

	/// Returns true if this is a key press event.
	[[nodiscard]] constexpr bool IsPressed() const noexcept
	{
		return State == ButtonState::Pressed;
	}

	/// Returns true if this is a key release event.
	[[nodiscard]] constexpr bool IsReleased() const noexcept
	{
		return State == ButtonState::Released;
	}

	/// Returns true if the specified modifier is active.
	[[nodiscard]] constexpr bool HasModifier(ModifierFlags flag) const noexcept
	{
		return HasAnyFlag(Modifiers, flag);
	}
};

}  // namespace Input
