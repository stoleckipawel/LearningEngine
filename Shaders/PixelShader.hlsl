#include "RootSignature.hlsl"

struct PSInput
{
    float4 position : SV_Position;
};

float3 Color : register(b0);

[RootSignature(ROOT_SIGNATURE)]
float4 main(in PSInput Input) : SV_Target
{
	return float4(0.5f, 0.5f, 1.0f, 1.0f);
}