// ============================================================================
// AnimationAPI.h - SparkleAnimation Module Export Macros
// ============================================================================
#pragma once

// DLL Export/Import macros for SparkleAnimation
#ifdef SPARKLE_STATIC
    #define SPARKLE_ANIMATION_API
#else
    #ifdef SPARKLE_ANIMATION_EXPORTS
        #define SPARKLE_ANIMATION_API __declspec(dllexport)
    #else
        #define SPARKLE_ANIMATION_API __declspec(dllimport)
    #endif
#endif
