// =============================================================================
// InputTypes.h — Convenience Header for All Input Types
// =============================================================================
//
// Single include for all input system types. Include this header when you need
// access to multiple input types. For minimal dependencies, include individual
// headers directly.
//
// Structure:
//   Core/Input/
//   ├── InputTypes.h          ← This file (convenience header)
//   ├── Keyboard/
//   │   ├── Key.h             ← Key codes + classification utilities
//   │   └── ModifierFlags.h   ← Modifier bitmask + operators
//   ├── Mouse/
//   │   ├── MouseButton.h     ← Mouse button identifiers
//   │   ├── MouseWheel.h      ← Wheel state + delta tracking
//   │   └── MousePosition.h   ← Position + normalized coords
//   ├── State/
//   │   └── ButtonState.h     ← Button state (Up/Pressed/Held/Released)
//   ├── Device/
//   │   └── InputDevice.h     ← Device type identifiers
//   ├── Dispatch/
//   │   ├── InputLayer.h      ← Priority layers
//   │   └── DispatchMode.h    ← Event dispatch timing
//   └── Events/
//       ├── InputEvent.h      ← Convenience + InputEventVariant
//       ├── KeyboardEvent.h   ← Keyboard key press/release
//       ├── MouseButtonEvent.h← Mouse button press/release
//       ├── MouseMoveEvent.h  ← Mouse cursor movement
//       └── MouseWheelEvent.h ← Mouse wheel scroll
//
// =============================================================================

#pragma once

// Keyboard
#include "Keyboard/Key.h"
#include "Keyboard/ModifierFlags.h"

// Mouse
#include "Mouse/MouseButton.h"
#include "Mouse/MouseWheel.h"
#include "Mouse/MousePosition.h"

// State
#include "State/ButtonState.h"

// Device
#include "Device/InputDevice.h"

// Dispatch
#include "Dispatch/InputLayer.h"
#include "Dispatch/DispatchMode.h"

// Events
#include "Events/InputEvent.h"

