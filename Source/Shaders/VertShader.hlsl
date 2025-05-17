

struct VsInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VsOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};


cbuffer ConstantBuffer : register(b0)
{
    float4 color;
};


void main(in VsInput Input, out VsOutput Output) 
{
    Output.Position = float4(Input.Position.xyz, 1.0f);
    Output.Color = Input.Color * color;
}
