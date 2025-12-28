
#include "Common.hlsli"

void main(in PsInput Input, out PsOutput Output)
{
	UnpackPsInput(Input);

	const float3 DiffuseAlbedo = Material::CalculateDiffuseAlbedo(Input);
	const float3 DirectLighting = Lighting::CalculateDirect(Input.NormalWorld);
	const float3 IndirectLighting = Lighting::CalculateIndireect();

	Output.Color0.xyz = DiffuseAlbedo * (DirectLighting + IndirectLighting);

	// Output.Color0.xyz = Input.NormalWorld.xyz * 0.5f + 0.5f;
	Output.Color0.w = 1.0f;
}
