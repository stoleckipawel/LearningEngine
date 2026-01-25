// =============================================================================
// MouseWheelEvent.h — Mouse Wheel Scroll Event
// =============================================================================
//
// Event fired when the mouse wheel is scrolled (vertical or horizontal).
//
// =============================================================================

#pragma once

#include "../Mouse/MousePosition.h"
#include "../Mouse/MouseWheel.h"
#include "../Keyboard/ModifierFlags.h"
#include "../Device/InputDevice.h"

namespace Input
{

// =============================================================================
// MouseWheelEvent
// =============================================================================

/// Event data for mouse wheel scroll.
struct MouseWheelEvent
{
	float Delta = 0.0f;          ///< Scroll amount (+1.0 = one notch up/right)
	MouseWheelAxis Axis = MouseWheelAxis::Vertical;  ///< Vertical or horizontal wheel
	MousePosition Position;      ///< Cursor position at time of scroll
	ModifierFlags Modifiers = ModifierFlags::None;   ///< Active keyboard modifiers

	/// Returns the device type (always Mouse).
	[[nodiscard]] constexpr InputDevice GetDevice() const noexcept
	{
		return InputDevice::Mouse;
	}

	/// Returns true if this is a vertical wheel event.
	[[nodiscard]] constexpr bool IsVertical() const noexcept
	{
		return Axis == MouseWheelAxis::Vertical;
	}

	/// Returns true if this is a horizontal wheel event.
	[[nodiscard]] constexpr bool IsHorizontal() const noexcept
	{
		return Axis == MouseWheelAxis::Horizontal;
	}

	/// Returns true if scrolling up/right (positive direction).
	[[nodiscard]] constexpr bool IsPositive() const noexcept
	{
		return Delta > 0.0f;
	}

	/// Returns true if scrolling down/left (negative direction).
	[[nodiscard]] constexpr bool IsNegative() const noexcept
	{
		return Delta < 0.0f;
	}

	/// Returns true if the specified modifier is active.
	[[nodiscard]] constexpr bool HasModifier(ModifierFlags flag) const noexcept
	{
		return HasAnyFlag(Modifiers, flag);
	}
};

}  // namespace Input
