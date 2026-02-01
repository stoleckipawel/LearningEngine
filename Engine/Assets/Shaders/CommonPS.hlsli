#pragma once

// =============================================================================
// Common Pixel Shader Include
// =============================================================================
// Lightweight include for pixel shaders.

#include "Common/Constants.hlsli"
#include "Common/Math.hlsli"
#include "Resources/ConstantBuffers.hlsli"

#include "Geometry/PixelInput.hlsli"
#include "Geometry/PixelOutput.hlsli"
#include "Material/Material.hlsli"
#include "BRDF/BRDF.hlsli"
#include "Lighting/LightEvaluation.hlsli"
#include "Passes/Forward/ForwardLighting.hlsli"

#include "Debug/ViewModes.hlsli"
