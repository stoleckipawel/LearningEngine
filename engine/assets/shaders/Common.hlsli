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
// Common - Foundation utilities
// -----------------------------------------------------------------------------
#include "Common/Constants.hlsli"
#include "Common/Math.hlsli"

// -----------------------------------------------------------------------------
// Resources - GPU resource bindings
// -----------------------------------------------------------------------------
#include "Resources/ConstantBuffers.hlsli"

// -----------------------------------------------------------------------------
// Geometry - Vertex/pixel shader IO and transforms
// -----------------------------------------------------------------------------
#include "Geometry/VertexInput.hlsli"
#include "Geometry/VertexOutput.hlsli"
#include "Geometry/PixelInput.hlsli"
#include "Geometry/PixelOutput.hlsli"
#include "Geometry/Transforms.hlsli"

// -----------------------------------------------------------------------------
// Material - Surface properties
// -----------------------------------------------------------------------------
#include "Material/Material.hlsli"

// -----------------------------------------------------------------------------
// BRDF - Physically-based shading models
// -----------------------------------------------------------------------------
#include "BRDF/BRDF.hlsli"

// -----------------------------------------------------------------------------
// Lighting - Light evaluation
// -----------------------------------------------------------------------------
#include "Lighting/LightEvaluation.hlsli"

// -----------------------------------------------------------------------------
// Debug - Visualization tools
// -----------------------------------------------------------------------------
#include "Debug/ViewModes.hlsli"
