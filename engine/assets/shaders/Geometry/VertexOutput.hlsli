#pragma once

// =============================================================================
// Vertex Shader Output / Pixel Shader Input
// =============================================================================
// Data passed from vertex shader to pixel shader via interpolators

namespace VS
{
	struct Output
	{
		float2 TexCoord : TEXCOORD0;
		float3 PositionWorld : TEXCOORD1;
		float4 Color : COLOR;
		float3 NormalWorld : NORMAL;
		float4 TangentWorld : TANGENT;
		float3 BitangentWorld : BINORMAL;
		float4 Position : SV_POSITION;  // Must be last (clip-space position)
	};
}  // namespace VS
