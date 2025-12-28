#pragma once

struct PsInput
{
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
	float3 NormalWorld : NORMAL;
	float3 TangentWorld : TANGENT;
	float3 BitangentWorld : BINORMAL;
};

struct PsOutput
{
	float4 Color0 : SV_Target;
};

void UnpackPsInput(inout PsInput Input)
{
	// Ensure normal is normalized, (interpolation issues)
	Input.NormalWorld = normalize(Input.NormalWorld);
}