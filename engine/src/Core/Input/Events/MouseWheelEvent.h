// =============================================================================
// MouseWheelEvent.h — Mouse Wheel Scroll Event
// =============================================================================
//
// Event fired when the mouse wheel is scrolled (vertical or horizontal).
//
// =============================================================================

#pragma once

#include "../Mouse/MousePosition.h"
#include "../Device/InputDevice.h"

// =============================================================================
// MouseWheelEvent
// =============================================================================

/// Event data for mouse wheel scroll.
struct MouseWheelEvent
{
	float Delta = 0.0f;          ///< Scroll amount (+1.0 = one notch up/right)
	MousePosition Position;      ///< Cursor position at time of scroll
	bool bHorizontal = false;    ///< True = horizontal (tilt), False = vertical

	/// Returns the device type (always Mouse).
	[[nodiscard]] constexpr InputDevice GetDevice() const noexcept
	{
		return InputDevice::Mouse;
	}

	/// Returns true if this is a vertical wheel event.
	[[nodiscard]] constexpr bool IsVertical() const noexcept
	{
		return !bHorizontal;
	}

	/// Returns true if this is a horizontal wheel event.
	[[nodiscard]] constexpr bool IsHorizontal() const noexcept
	{
		return bHorizontal;
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
};
