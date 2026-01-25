// =============================================================================
// DispatchMode.h — Event Dispatch Timing
// =============================================================================
//
// Controls when event callbacks are invoked.
//
// Immediate: Callback fires synchronously inside WndProc/message handler.
//            Use for low-latency responses (e.g., UI clicks, debug toggles).
//
// Deferred:  Event is queued and callback fires during ProcessDeferredEvents().
//            Use for high-frequency events (e.g., mouse move) to batch processing
//            and avoid stalling the message pump.
//
// =============================================================================

#pragma once

#include <cstdint>

namespace Input
{

// =============================================================================
// DispatchMode — Event Dispatch Timing
// =============================================================================

enum class DispatchMode : std::uint8_t
{
	Immediate,  // Fire synchronously in WndProc context
	Deferred,   // Queue for processing in BeginFrame
};

}  // namespace Input
