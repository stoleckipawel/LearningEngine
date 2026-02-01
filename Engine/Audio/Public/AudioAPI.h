// ============================================================================
// AudioAPI.h - SparkleAudio Module Export Macros
// ============================================================================
#pragma once

// DLL Export/Import macros for SparkleAudio
#ifdef SPARKLE_STATIC
	#define SPARKLE_AUDIO_API
#else
	#ifdef SPARKLE_AUDIO_EXPORTS
		#define SPARKLE_AUDIO_API __declspec(dllexport)
	#else
		#define SPARKLE_AUDIO_API __declspec(dllimport)
	#endif
#endif
