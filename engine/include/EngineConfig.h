#pragma once

// Engine-wide configuration constants.
// Keep very small and header-only so they are cheap to include everywhere.


#include <string>
#include <dxgi1_6.h>

// Compile-time toggle to completely exclude GUI from builds when set to 0.
#define USE_GUI 1
//#define USE_IMGUI_DEMO_WINDOW 1

// Shader compilation helpers. These are enabled by default for debug builds
// to include extra debug info and to ease shader debugging. 
#if defined(_DEBUG)
    #define ENGINE_SHADERS_OPTIMIZED 1
    #define ENGINE_SHADERS_DEBUG 1
#endif

// Enable GPU validation (D3D12/DXGI SDK layers). On in debug builds by default.
#if defined(_DEBUG)
    #define ENGINE_GPU_VALIDATION 1
#endif

// Report live D3D/DXGI objects at shutdown (useful for leak detection).
#if defined(_DEBUG)
    #define ENGINE_REPORT_LIVE_OBJECTS 1
#endif


namespace EngineSettings
{
    // Number of frames that can be processed simultaneously (frames in flight).
    // Increasing this reduces CPU-GPU synchronization but increases latency and
    // memory usage because more per-frame resources are required.
    inline constexpr unsigned FramesInFlight = 2u;

    // Back buffer format
    inline constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Presentation: when true, `SwapChain::Present` uses
    // vertical-sync. Setting to false allows uncapped presents (or tearing
    // if supported) and can reduce latency for benchmarking.
    inline bool VSync = true;

    // Start the application fullscreen when true.
    inline bool StartFullscreen = false;
    // Initial window title; application code may modify at runtime.
    inline std::string WindowTitle = "PlaygroundEngine";

    // Prefer high-performance adapter when enumerating GPUs. When true, queries
    // adapters by GPU preference; otherwise falls back to power-saving preference.
    inline bool PreferHighPerformanceAdapter = true;
}
