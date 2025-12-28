
#include "Common.hlsli"

void main(in PsInput Input, out PsOutput Output)
{
	UnpackPsInput(Input);

	Material::Properties MatProps = Material::SampleMaterial(Input.TexCoord);

	const float3 DirectLighting = Lighting::CalculateDirect(Input.NormalWorld);
	const float3 IndirectLighting = Lighting::CalculateIndireect();

	Output.Color0.xyz = MatProps.BaseColor * (DirectLighting + IndirectLighting);

	Output.Color0.w = 1.0f;
}
