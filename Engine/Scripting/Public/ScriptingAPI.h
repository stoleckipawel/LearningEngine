// ============================================================================
// ScriptingAPI.h - SparkleScripting Module Export Macros
// ============================================================================
#pragma once

// DLL Export/Import macros for SparkleScripting
#ifdef SPARKLE_STATIC
    #define SPARKLE_SCRIPTING_API
#else
    #ifdef SPARKLE_SCRIPTING_EXPORTS
        #define SPARKLE_SCRIPTING_API __declspec(dllexport)
    #else
        #define SPARKLE_SCRIPTING_API __declspec(dllimport)
    #endif
#endif
