


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

Texture2D<float4> myTexture : register(t0);
sampler textureSampler : register(s0);

void main(in PsInput Input, out PsOutput Output)
{
    float4 texel = myTexture.Sample(textureSampler, Input.TexCoord);
    Output.Color0 = float4(Input.TexCoord, 0.0, 1.0f);
    Output.Color0 = float4(texel.rgb, 1.0f);
}
