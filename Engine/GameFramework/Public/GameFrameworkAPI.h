// ============================================================================
// GameFrameworkAPI.h - SparkleEngine Module Export Macros
// ============================================================================
// This module contains the game framework: World, Entities, Components, Assets
// Corresponds to SparkleEngine.dll in the Unreal-style architecture
// ============================================================================
#pragma once

// DLL Export/Import macros for SparkleEngine (GameFramework)
#ifdef SPARKLE_STATIC
    #define SPARKLE_ENGINE_API
#else
    #ifdef SPARKLE_ENGINE_EXPORTS
        #define SPARKLE_ENGINE_API __declspec(dllexport)
    #else
        #define SPARKLE_ENGINE_API __declspec(dllimport)
    #endif
#endif

// Legacy alias for backward compatibility during transition
#define SPARKLE_GAMEFRAMEWORK_API SPARKLE_ENGINE_API
