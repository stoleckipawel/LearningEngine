
#include "Common.hlsli"
#include "Viewmode.hlsli"

void main(in PsInput Input, out PsOutput Output)
{
	UnpackPsInput(Input);

	Material::Properties MatProps = Material::SampleMaterial(Input.TexCoord);

	const float3 DirectLighting = Lighting::CalculateDirect(Input.NormalWorld);
	const float3 IndirectLighting = Lighting::CalculateIndireect();


	const float3 DirectDiffuse = MatProps.BaseColor * DirectLighting;
	const float3 DirectSpecular = 0.0f;

	const float3 IndirectDiffuse = MatProps.BaseColor * IndirectLighting;
	const float3 IndirectSpecular = 0.0f;

	const float3 Lit = DirectDiffuse + IndirectDiffuse + MatProps.Emissive;

	const float3 FinalColor = ViewMode::Resolve(Lit,
	                                           MatProps,
	                                           DirectDiffuse,
	                                           DirectSpecular,
	                                           IndirectDiffuse,
	                                           IndirectSpecular);

	Output.Color0.xyz = FinalColor;
	Output.Color0.w = 1.0f;
}
