// ============================================================================
// AIAPI.h - SparkleAI Module Export Macros
// ============================================================================
#pragma once

// DLL Export/Import macros for SparkleAI
#ifdef SPARKLE_STATIC
	#define SPARKLE_AI_API
#else
	#ifdef SPARKLE_AI_EXPORTS
		#define SPARKLE_AI_API __declspec(dllexport)
	#else
		#define SPARKLE_AI_API __declspec(dllimport)
	#endif
#endif
