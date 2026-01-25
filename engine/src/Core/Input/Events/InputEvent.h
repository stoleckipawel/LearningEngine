// =============================================================================
// InputEvent.h — Input Event Types (Convenience Header)
// =============================================================================
//
// Convenience header for all input event types. Each event is a separate
// strongly-typed struct. The InputSystem uses separate typed arrays for
// deferred dispatch (Epic/Unreal style) rather than polymorphic containers.
//
// Structure:
//   Core/Input/Events/
//   ├── InputEvent.h          ← This file (convenience header)
//   ├── KeyboardEvent.h       ← Keyboard key press/release
//   ├── MouseButtonEvent.h    ← Mouse button press/release
//   ├── MouseMoveEvent.h      ← Mouse cursor movement
//   └── MouseWheelEvent.h     ← Mouse wheel scroll
//
// USAGE:
//   // Subscribe to specific event types with typed callbacks
//   inputSystem.OnKeyPressed([](const KeyboardEvent& e) {
//       if (e.KeyCode == Key::W) { /* move forward */ }
//   });
//
//   inputSystem.OnMouseMove([](const MouseMoveEvent& e) {
//       camera.Rotate(e.Delta.X, e.Delta.Y);
//   });
//
// =============================================================================

#pragma once

#include "KeyboardEvent.h"
#include "MouseButtonEvent.h"
#include "MouseMoveEvent.h"
#include "MouseWheelEvent.h"

