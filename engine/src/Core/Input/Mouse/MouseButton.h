// =============================================================================
// MouseButton.h — Mouse Button Identifiers
// =============================================================================
//
// Identifies mouse buttons. Supports standard 5-button mice.
//
// =============================================================================

#pragma once

#include <cstdint>

namespace Input
{

// =============================================================================
// MouseButton — Mouse Button Identifiers
// =============================================================================

enum class MouseButton : std::uint8_t
{
	Left = 0,
	Right = 1,
	Middle = 2,
	Button4 = 3,  // XButton1 (back/side)
	Button5 = 4,  // XButton2 (forward/side)

	Count = 5
};

}  // namespace Input
