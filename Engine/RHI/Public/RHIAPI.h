// RHIAPI.h - SparkleRHI DLL export/import macros
#pragma once

// DLL export/import configuration
#ifdef SPARKLE_RHI_EXPORTS
	#define SPARKLE_RHI_API __declspec(dllexport)
#else
	#define SPARKLE_RHI_API __declspec(dllimport)
#endif

// For static library builds
#ifdef SPARKLE_STATIC
	#undef SPARKLE_RHI_API
	#define SPARKLE_RHI_API
#endif
