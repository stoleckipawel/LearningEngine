

struct VsInput
{
    float3 Position : POSITION;
    float2 UV0 : TEXCOORD;
};

struct VsOutput
{
    float2 UV0 : TEXCOORD;
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
    Output.UV0 = Input.UV0;
}
