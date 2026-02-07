// =============================================================================
// Forward Lit Vertex Shader
// =============================================================================
// Basic vertex transformation with world-space outputs for PBR lighting.

#include "CommonVS.hlsli"

void main(in VS::Input Input, out VS::Output Output)
{
	// Transform local-space vectors to world space
	const float4 positionWorld = PositionLocalToWorld(float4(Input.Position, 1.0f));
	const float3 normalWorld = NormalLocalToWorld(Input.Normal);
	const float4 tangentWorld = TangentLocalToWorld(Input.Tangent);

	// Reconstruct bitangent from world normal/tangent
	const float3 bitangentWorld = ComputeBitangent(normalWorld, tangentWorld);

	// Transform to clip space
	const float4 positionClip = PositionWorldToClip(positionWorld);

	// Output
	Output.Position = positionClip;
	Output.PositionWorld = positionWorld.xyz;
	Output.NormalWorld = normalWorld;
	Output.TangentWorld = tangentWorld;
	Output.BitangentWorld = bitangentWorld;
	Output.TexCoord = Input.TexCoord;
	Output.Color = Input.Color;
}
