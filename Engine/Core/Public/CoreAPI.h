// CoreAPI.h - SparkleCore DLL export/import macros
#pragma once

// DLL export/import configuration
#ifdef SPARKLE_CORE_EXPORTS
	#define SPARKLE_CORE_API __declspec(dllexport)
#else
	#define SPARKLE_CORE_API __declspec(dllimport)
#endif

// For static library builds
#ifdef SPARKLE_STATIC
	#undef SPARKLE_CORE_API
	#define SPARKLE_CORE_API
#endif
