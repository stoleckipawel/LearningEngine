#include "Common.hlsli"

struct VsInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
};

struct VsOutput
{
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
	float4 Position : SV_POSITION;
};

void main(in VsInput Input, out VsOutput Output)
{
	float speed = FrameIndex * 0.005;

	const float PI = 3.14159265358979323846f;
	float rotationAngle = speed * 2.0f * PI;  // one full rotation per configured period
	float cosAngle = cos(rotationAngle);
	float sinAngle = sin(rotationAngle);
	float3 inputPosition = Input.Position;
	float3 rotatedPosition = float3(
	    cosAngle * inputPosition.x + sinAngle * inputPosition.z,
	    inputPosition.y,
	    -sinAngle * inputPosition.x + cosAngle * inputPosition.z);

	float4 localPosition = float4(rotatedPosition, 1.0f);

	float4 worldPosition = mul(localPosition, WorldMTX);
	float4 viewPosition = mul(worldPosition, ViewMTX);
	float4 clipPosition = mul(viewPosition, ProjectionMTX);

	Output.Position = clipPosition;
	Output.TexCoord = Input.TexCoord;
	Output.Color = Input.Color;
}
