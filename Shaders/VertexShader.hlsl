#include "RootSignature.hlsl"
#include "Pipeline.hlsli"



[RootSignature(ROOTSIG)]
void main(
          in float3 pos : Position,
          in float2 uv : TexCoord,

          out float2 OutUv : TexCoord,
          out float4 OutPos : SV_Position
) 
{
    OutPos = float4(pos.xyz, 1.0f);
    
    OutUv = uv.xy;
}