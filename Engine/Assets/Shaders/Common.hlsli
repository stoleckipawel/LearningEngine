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
#include "Common/Constants.hlsli"
#include "Common/Math.hlsli"

// -----------------------------------------------------------------------------
// resources - GPU resource bindings
// -----------------------------------------------------------------------------
#include "Resources/ConstantBuffers.hlsli"

// -----------------------------------------------------------------------------
// geometry - Vertex/pixel shader IO and transforms
// -----------------------------------------------------------------------------
#include "Geometry/VertexInput.hlsli"
#include "Geometry/VertexOutput.hlsli"
#include "Geometry/PixelInput.hlsli"
#include "Geometry/PixelOutput.hlsli"
#include "Geometry/Transforms.hlsli"

// -----------------------------------------------------------------------------
// material - Surface properties
// -----------------------------------------------------------------------------
#include "Material/Material.hlsli"

// -----------------------------------------------------------------------------
// brdf - Physically-based shading models
// -----------------------------------------------------------------------------
#include "BRDF/BRDF.hlsli"

// -----------------------------------------------------------------------------
// lighting - Light evaluation
// -----------------------------------------------------------------------------
#include "Lighting/LightEvaluation.hlsli"

// -----------------------------------------------------------------------------
// debug - Visualization tools
// -----------------------------------------------------------------------------
#include "Debug/ViewModes.hlsli"
