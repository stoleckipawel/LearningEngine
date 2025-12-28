
#include "Common.hlsli"

void main(in PsInput Input, out PsOutput Output)
{
	UnpackPsInput(Input);

	const float3 DiffuseAlbedo = Material::CalculateDiffuseAlbedo(Input);
	const float DirectLighting = Lighting::CalculateDirect(Input.NormalWorld);

	Output.Color0.xyz = DiffuseAlbedo * DirectLighting;

	// Output.Color0.xyz = Input.NormalWorld.xyz * 0.5f + 0.5f;
	Output.Color0.w = 1.0f;
}
