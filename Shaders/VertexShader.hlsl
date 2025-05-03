#include "RootSignature.hlsl"

[RootSignature(ROOTSIG)]
void main(
          in float2 pos : Position,
          in float2 uv : TexCoord,

          out float2 OutUv : TexCoord,
          out float4 OutPos : SV_Position
) 
{
    OutUv = uv.xy;
    OutPos = float4(pos.xy, 0.0f, 1.0f);
}