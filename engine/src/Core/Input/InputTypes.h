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
//   │   └── MouseButton.h     ← Mouse button identifiers
//   ├── State/
//   │   └── ButtonState.h     ← Button state (Up/Pressed/Held/Released)
//   ├── Device/
//   │   └── InputDevice.h     ← Device type identifiers
//   └── Dispatch/
//       ├── InputLayer.h      ← Priority layers
//       └── DispatchMode.h    ← Event dispatch timing
//
// =============================================================================

#pragma once

// Keyboard
#include "Keyboard/Key.h"
#include "Keyboard/ModifierFlags.h"

// Mouse
#include "Mouse/MouseButton.h"
#include "Mouse/MouseWheel.h"

// State
#include "State/ButtonState.h"

// Device
#include "Device/InputDevice.h"

// Dispatch
#include "Dispatch/InputLayer.h"
#include "Dispatch/DispatchMode.h"

