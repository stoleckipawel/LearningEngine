

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
    Output.Position = float4(Input.Position.xyz, 1.0f);
    Output.TexCoord = Input.TexCoord;
    Output.Color = Input.Color;

}
