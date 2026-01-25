// =============================================================================
// MouseMoveEvent.h — Mouse Movement Event
// =============================================================================
//
// Event fired when the mouse cursor moves.
//
// =============================================================================

#pragma once

#include "../Mouse/MousePosition.h"
#include "../Keyboard/ModifierFlags.h"
#include "../Device/InputDevice.h"

namespace Input
{

// =============================================================================
// MouseMoveEvent
// =============================================================================

/// Event data for mouse cursor movement.
struct MouseMoveEvent
{
	MousePosition Position;    ///< Current cursor position
	MouseDelta Delta;          ///< Movement since last event
	ModifierFlags Modifiers = ModifierFlags::None;  ///< Active keyboard modifiers

	/// Returns the device type (always Mouse).
	[[nodiscard]] constexpr InputDevice GetDevice() const noexcept
	{
		return InputDevice::Mouse;
	}

	/// Returns true if the specified modifier is active.
	[[nodiscard]] constexpr bool HasModifier(ModifierFlags flag) const noexcept
	{
		return HasAnyFlag(Modifiers, flag);
	}
};

}  // namespace Input
