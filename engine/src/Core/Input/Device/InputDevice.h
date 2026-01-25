// =============================================================================
// InputDevice.h — Input Device Type Identifiers
// =============================================================================
//
// Identifies the source device of an input event.
// Enables consumers to filter or handle input differently per device.
//
// =============================================================================

#pragma once

#include <cstdint>

namespace Input
{

// =============================================================================
// InputDevice — Device Type Identifiers
// =============================================================================

enum class InputDevice : std::uint8_t
{
	Keyboard = 0,
	Mouse,
	Gamepad,  
	Touch,   

	Count
};

}  // namespace Input
