// ============================================================================
// Events.h — Convenience Header
// ----------------------------------------------------------------------------
// Single include for the complete event system.
//
// Structure:
//   Core/Events/
//   ├── Events.h           ← This file (convenience header)
//   ├── EventHandle.h      ← Subscription handle
//   ├── Event.h            ← Multicast event dispatcher
//   └── ScopedEventHandle.h← RAII auto-unsubscribe wrapper
//
// USAGE:
//   #include "Core/Events/Events.h"
//
//   Event<void(int, float)> OnValueChanged;
//   auto handle = OnValueChanged.Add([](int a, float b) { ... });
//   OnValueChanged.Broadcast(42, 3.14f);
//
//   // RAII auto-unsubscribe:
//   ScopedEventHandle scoped(OnValueChanged, handle);
//
// ============================================================================

#pragma once

#include "EventHandle.h"
#include "Event.h"
#include "ScopedEventHandle.h"
