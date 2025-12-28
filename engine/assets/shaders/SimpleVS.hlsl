
#include "Common.hlsli"

void main(in VsInput Input, out VsOutput Output)
{
	const float rotationOffset = FrameIndex * 0.005f;
	const float3 rotationAxis = float3(0.0f, 1.0f, 0.0f);

	// 1) Rotate position, normal, and tangent in LOCAL space first
	const float3 positionLocalRotated = Rotate(Input.Position, rotationAxis, rotationOffset);
	const float3 normalLocalRotated = Rotate(Input.Normal, rotationAxis, rotationOffset);
	const float3 tangentLocalRotated = Rotate(Input.Tangent.xyz, rotationAxis, rotationOffset);

	// 2) Transform rotated local-space vectors to world space
	const float4 positionWorld = PositionLocalToWorld(float4(positionLocalRotated, 1.0f));
	const float3 normalWorld = UnpackNormalWorld(normalLocalRotated);
	const float4 tangentWorld = UnpackTangentWorld(float4(tangentLocalRotated, Input.Tangent.w));

	// 3) Reconstruct bitangent from world normal/tangent (uses handedness in tangent.w)
	const float3 bitangentWorld = UnpackBitangentWorld(normalWorld, tangentWorld);

	// 4) Transform to clip space
	const float4 positionView = PositionWorldToView(positionWorld);
	const float4 positionClip = PositionViewToClip(positionView);

	Output.Position = positionClip;
	Output.NormalWorld = normalWorld;
	Output.TangentWorld = tangentWorld;
	Output.BitangentWorld = bitangentWorld;
	Output.TexCoord = Input.TexCoord;
	Output.Color = Input.Color;
}
