#pragma once

#include "LayoutPixel.hlsli"

Texture2D myTexture : register(t0);
sampler textureSampler : register(s0);

namespace Material
{
float3 CalculateDiffuseAlbedo(PsInput Input)
{
	float3 Albedo = myTexture.SampleLevel(textureSampler, Input.TexCoord, 0.0f).xyz;
	// Albedo *= BaseColor.rgb;
	Albedo *= Input.Color.rgb;
	return Albedo;
}
}  // namespace Material