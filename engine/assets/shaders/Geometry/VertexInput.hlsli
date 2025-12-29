#pragma once

// =============================================================================
// Vertex Shader Input Layout
// =============================================================================
// Defines the structure expected from the input assembler (mesh data)

namespace VS
{
	struct Input
	{
		float3 Position : POSITION;
		float2 TexCoord : TEXCOORD;
		float4 Color : COLOR;
		float3 Normal : NORMAL;
		float4 Tangent : TANGENT;  // xyz = tangent, w = handedness (+1 or -1)
	};
}  // namespace VS
