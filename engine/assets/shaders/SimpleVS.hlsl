

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

cbuffer VertexConstantBuffer : register(b0)
{
    float4x4 WorldMTX;
    float4x4 ViewMTX;
    float4x4 ProjectionMTX;
};

void main(in VsInput Input, out VsOutput Output) 
{
    float4 LocalPosition = float4(Input.Position.xyz, 1.0f);
    float4 WorldPosition = mul(LocalPosition, WorldMTX);
    float4 ViewPosition = mul(WorldPosition, ViewMTX);
    float4 ProjPosition = mul(ViewPosition, ProjectionMTX);
    Output.Position = ViewPosition;
    Output.TexCoord = Input.TexCoord;
    Output.Color = Input.Color;
}
