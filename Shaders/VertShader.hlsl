

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

/*
struct VSConstants
{
    //Make sure to allign to 4x 32 bits
    float4x4 ModelViewProjection_XFORM;
};


//ConstantBuffer<VSConstants> VsConstantBuffer : register(b4);
*/

void main(in VsInput Input, out VsOutput Output) 
{
    Output.Position = float4(Input.Position.xyz, 1.0f);
    Output.Color = Input.Color;
}
