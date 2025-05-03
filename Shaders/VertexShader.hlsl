#include "RootSignature.hlsl"
#include "Pipeline.hlsli"

Correction correction : register(b1);

[RootSignature(ROOTSIG)]
void main(
          in float2 pos : Position,
          in float2 uv : TexCoord,

          out float2 OutUv : TexCoord,
          out float4 OutPos : SV_Position
) 
{
    float2 position = pos.xy;
    //position.x = (position.x * correction.cosAngle) - (position.x * correction.sinAngle);
    //position.y = (position.y * correction.sinAngle) + (position.y * correction.cosAngle);

    //position *= correction.Zoom;
    
    //position.x *= correction.AspectRatio;
    
    OutPos = float4(position, 0.0f, 1.0f);
    
    OutUv = uv.xy;
}