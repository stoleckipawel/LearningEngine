// ============================================================================
// PhysicsAPI.h - SparklePhysics Module Export Macros
// ============================================================================
#pragma once

// DLL Export/Import macros for SparklePhysics
#ifdef SPARKLE_STATIC
	#define SPARKLE_PHYSICS_API
#else
	#ifdef SPARKLE_PHYSICS_EXPORTS
		#define SPARKLE_PHYSICS_API __declspec(dllexport)
	#else
		#define SPARKLE_PHYSICS_API __declspec(dllimport)
	#endif
#endif
