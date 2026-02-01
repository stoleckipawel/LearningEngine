// ============================================================================
// NetworkingAPI.h - SparkleNetworking Module Export Macros
// ============================================================================
#pragma once

// DLL Export/Import macros for SparkleNetworking
#ifdef SPARKLE_STATIC
	#define SPARKLE_NETWORKING_API
#else
	#ifdef SPARKLE_NETWORKING_EXPORTS
		#define SPARKLE_NETWORKING_API __declspec(dllexport)
	#else
		#define SPARKLE_NETWORKING_API __declspec(dllimport)
	#endif
#endif
