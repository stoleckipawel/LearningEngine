
#include "Common.hlsli"
#include "Viewmode.hlsli"

void main(in PsInput Input, out PsOutput Output)
{
	UnpackPsInput(Input);

	Material::Properties MatProps = Material::SampleMaterial(Input.TexCoord);
	MatProps.NormalWorld = Input.NormalWorld;

	const float3 DirectLighting = Lighting::CalculateDirect(Input.NormalWorld);
	const float3 IndirectLighting = Lighting::CalculateIndireect();


	const float3 DirectDiffuse = MatProps.BaseColor * DirectLighting;
	const float3 DirectSpecular = 0.0f;

	const float3 IndirectDiffuse = MatProps.BaseColor * IndirectLighting;
	const float3 IndirectSpecular = 0.0f;

	const float3 Lit = DirectDiffuse + IndirectDiffuse + MatProps.Emissive;

	const float3 FinalColor = ViewMode::Resolve(Lit, MatProps, DirectDiffuse, DirectSpecular, IndirectDiffuse, IndirectSpecular);

	// Debug: Show ViewModeIndex as color to verify CB is being read
	// Remove this once confirmed working
	float3 debugColor = float3(ViewModeIndex / 10.0f, 0.0f, 1.0f - ViewModeIndex / 10.0f);
	Output.Color0.xyz = debugColor;

	Output.Color0.xyz = FinalColor;
	Output.Color0.w = 1.0f;
}
