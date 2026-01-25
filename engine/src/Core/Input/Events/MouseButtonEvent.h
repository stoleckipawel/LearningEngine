// =============================================================================
// MouseButtonEvent.h — Mouse Button Input Event
// =============================================================================
//
// Event fired when a mouse button is pressed or released.
//
// =============================================================================

#pragma once

#include "../Mouse/MouseButton.h"
#include "../Mouse/MousePosition.h"
#include "../Keyboard/ModifierFlags.h"
#include "../State/ButtonState.h"
#include "../Device/InputDevice.h"

namespace Input
{

// =============================================================================
// MouseButtonEvent
// =============================================================================

/// Event data for mouse button press/release.
struct MouseButtonEvent
{
	MouseButton Button = MouseButton::Left;       ///< Which button was pressed/released
	ButtonState State = ButtonState::Up;          ///< Press or release
	MousePosition Position;                       ///< Cursor position at time of event
	ModifierFlags Modifiers = ModifierFlags::None;  ///< Active keyboard modifiers

	/// Returns the device type (always Mouse).
	[[nodiscard]] constexpr InputDevice GetDevice() const noexcept
	{
		return InputDevice::Mouse;
	}

	/// Returns true if this is a button press event.
	[[nodiscard]] constexpr bool IsPressed() const noexcept
	{
		return State == ButtonState::Pressed;
	}

	/// Returns true if this is a button release event.
	[[nodiscard]] constexpr bool IsReleased() const noexcept
	{
		return State == ButtonState::Released;
	}

	/// Returns true if the specified modifier is active.
	[[nodiscard]] constexpr bool HasModifier(ModifierFlags flag) const noexcept
	{
		return HasAnyFlag(Modifiers, flag);
	}

	/// Returns true if this is a left button event.
	[[nodiscard]] constexpr bool IsLeftButton() const noexcept
	{
		return Button == MouseButton::Left;
	}

	/// Returns true if this is a right button event.
	[[nodiscard]] constexpr bool IsRightButton() const noexcept
	{
		return Button == MouseButton::Right;
	}

	/// Returns true if this is a middle button event.
	[[nodiscard]] constexpr bool IsMiddleButton() const noexcept
	{
		return Button == MouseButton::Middle;
	}
};

}  // namespace Input
