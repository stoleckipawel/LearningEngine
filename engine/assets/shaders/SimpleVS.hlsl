
#include "Common.hlsli"

void main(in VsInput Input, out VsOutput Output)
{
	const float4 positionClip = PositionLocalToClip(float4(Input.Position, 1.0f));
	const float3 normalWorld = UnpackNormalWorld(Input.Normal);
	const float4 tangentWorld = UnpackTangentWorld(Input.Tangent);
	const float3 bitangentWorld = UnpackBitangentWorld(normalWorld, tangentWorld);

	Output.Position = positionClip;
	Output.NormalWorld = normalWorld;
	Output.TangentWorld = tangentWorld;
	Output.BitangentWorld = bitangentWorld;
	Output.TexCoord = Input.TexCoord;
	Output.Color = Input.Color; 
}
