#pragma once

// =============================================================================
// Common Pixel Shader Include
// =============================================================================
// Lightweight include for pixel shaders.

#include "common/constants.hlsli"
#include "common/math.hlsli"
#include "resources/constantbuffers.hlsli"

#include "geometry/pixelinput.hlsli"
#include "geometry/pixeloutput.hlsli"
#include "material/material.hlsli"
#include "brdf/brdf.hlsli"
#include "lighting/lightevaluation.hlsli"
#include "passes/forward/forwardlighting.hlsli"

#include "debug/viewmodes.hlsli"
