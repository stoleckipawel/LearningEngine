#include "RootSignature.hlsl"

struct VSInput
{
    float3 pos : POSITION;
};

[RootSignature(ROOT_SIGNATURE)]
float4 main(in VSInput Input) : SV_Position
{
    return float4(Input.pos, 1.0);
}