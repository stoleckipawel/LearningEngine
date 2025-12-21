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
    float4 LocalPosition = float4(Input.Position.xyz, 1.0f);
    float4 WorldPosition = mul(LocalPosition, WorldMTX);
    float4 ViewPosition = mul(WorldPosition, ViewMTX);
    float4 ClipPosition = mul(ViewPosition, ProjectionMTX);
    
    Output.Position = ClipPosition;
    Output.TexCoord = Input.TexCoord;
    Output.Color = Input.Color;
}
