#pragma once

// Engine-wide configuration constants.
// Keep very small and header-only so they are cheap to include everywhere.


#include <string>
#include <dxgi1_6.h>

// Compile-time toggle to completely exclude GUI from builds when set to 0.
#ifndef USE_GUI
    #define USE_GUI 1
#endif

namespace EngineSettings
{
    // Number of frames that can be processed simultaneously (frames in flight).
    // Increase to reduce CPU-GPU synchronization stalls at the cost of higher memory usage and latency.
    inline constexpr unsigned FramesInFlight = 2u;

    // Back buffer format
    inline constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Presentation (runtime toggle)
    inline bool VSync = true;                    // SwapChain::Present will use vsync when true

    // Window (runtime)
    inline bool StartFullscreen = false;
    inline std::string WindowTitle = "PlaygroundEngine";
}
