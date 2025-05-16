//float3 color : register(b0);
//Texture2D<float4> textures[] : register(t0);
//sampler texturesSampler : register(s0);

struct PsInput
{
    float2 UV0 : TEXCOORD;
};

struct PsOutput
{
    float4 Color0 : SV_Target;
};

void main(in PsInput Input, out PsOutput Output)
{
    //float4 texel = textures[0].Sample(texturesSampler, Input.UV0);
    //float4 texel = textures[0].Sample(texturesSampler, Input.UV0);
    Output.Color0 = float4(float3(1.0, 0.0, 0.0), 1.0f);
}
