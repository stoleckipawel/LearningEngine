
//Texture2D<float4> textures[] : register(t0);
//sampler texturesSampler : register(s0);

struct PsInput
{
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

struct PsOutput
{
    float4 Color0 : SV_Target;
};

cbuffer PixelConstantBuffer : register(b0)
{
    float4 color;
};

void main(in PsInput Input, out PsOutput Output)
{
    //float4 texel = textures[0].Sample(texturesSampler, Input.UV0);
    //float4 texel = textures[0].Sample(texturesSampler, Input.UV0);
    
    //Output.Color0 = float4(Input.TexCoord.xy, 0.0f, 1.0f);
    //Output.Color0 = float4(Input.Color.xyz, 1.0f);
    Output.Color0 = color;

}
