// =============================================================================
// Forward Lit Vertex Shader
// =============================================================================
// Basic vertex transformation with world-space outputs for PBR lighting.

#include "CommonVS.hlsli"

void main(in VS::Input Input, out VS::Output Output)
{
	// Procedural rotation
	const float rotationOffset = ScaledTotalTime * 0.3f;
	const float3 rotationAxis = float3(0.0f, 1.0f, 0.0f);
	const float3 positionLocalRotated = Rotate(Input.Position, rotationAxis, rotationOffset);
	const float3 normalLocalRotated = Rotate(Input.Normal, rotationAxis, rotationOffset);
	const float3 tangentLocalRotated = Rotate(Input.Tangent.xyz, rotationAxis, rotationOffset);

	// Transform rotated local-space vectors to world space
	const float4 positionWorld = PositionLocalToWorld(float4(positionLocalRotated, 1.0f));
	const float3 normalWorld = NormalLocalToWorld(normalLocalRotated);
	const float4 tangentWorld = TangentLocalToWorld(float4(tangentLocalRotated, Input.Tangent.w));

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
