#pragma once

struct VsInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
};

struct VsOutput
{
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
	float3 NormalWorld : NORMAL;
	float4 TangentWorld : TANGENT;
	float3 BitangentWorld : BINORMAL;	
	float4 Position : SV_POSITION;
};


float3 UnpackNormalWorld(in float3 NormalLocal)
{
	return normalize(mul(NormalLocal, WorldInvTransposeMTX));
}

float4 UnpackTangentWorld(float4 TangentLocal)
{
	// Transform tangent.xyz as a direction (3x3) and preserve handedness in w
	const float3 localTangent = TangentLocal.xyz;
	const float handedness = TangentLocal.w;
	const float3 worldTangent = mul(localTangent, (float3x3)WorldMTX);
	return float4(worldTangent, handedness);
}

float3 UnpackBitangentWorld(float3 NormalWorld, float4 TangentWorld)
{
	const float3 worldTangent = TangentWorld.xyz;
	const float handedness = TangentWorld.w;
	return handedness * normalize(cross(NormalWorld, worldTangent));
}
