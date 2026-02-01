#pragma once

// =============================================================================
// Common Shader Include
// =============================================================================
// Master include file that brings in all shared shader functionality.
//
// USAGE:
//   Vertex shaders: #include "CommonVS.hlsli"
//   Pixel shaders:  #include "CommonPS.hlsli"
//   Both:           #include "Common.hlsli"
// =============================================================================

// -----------------------------------------------------------------------------
// common - Foundation utilities
// -----------------------------------------------------------------------------
#include "common/constants.hlsli"
#include "common/math.hlsli"

// -----------------------------------------------------------------------------
// resources - GPU resource bindings
// -----------------------------------------------------------------------------
#include "resources/constantbuffers.hlsli"

// -----------------------------------------------------------------------------
// geometry - Vertex/pixel shader IO and transforms
// -----------------------------------------------------------------------------
#include "geometry/vertexinput.hlsli"
#include "geometry/vertexoutput.hlsli"
#include "geometry/pixelinput.hlsli"
#include "geometry/pixeloutput.hlsli"
#include "geometry/transforms.hlsli"

// -----------------------------------------------------------------------------
// material - Surface properties
// -----------------------------------------------------------------------------
#include "material/material.hlsli"

// -----------------------------------------------------------------------------
// brdf - Physically-based shading models
// -----------------------------------------------------------------------------
#include "brdf/brdf.hlsli"

// -----------------------------------------------------------------------------
// lighting - Light evaluation
// -----------------------------------------------------------------------------
#include "lighting/lightevaluation.hlsli"

// -----------------------------------------------------------------------------
// debug - Visualization tools
// -----------------------------------------------------------------------------
#include "debug/viewmodes.hlsli"
