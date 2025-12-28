#pragma once

#include "LayoutPixel.hlsli"

Texture2D TextureBaseColor : register(t0);
sampler SamplerLinearWrap : register(s0);

namespace Material
{
	struct Properties
	{
		float3 BaseColor;
		float3 NormalTangent;
		float3 NormalWorld;
		float Roughness;
		float3 Emissive;
		float Metallic;
		float AmbientOcclusion;
	};

	Properties MakeDefaultProperties()
	{
		Properties props;
		props.BaseColor = float3(1.0f, 1.0f, 1.0f);
		props.NormalTangent = float3(0.0f, 0.0f, 1.0f);
		props.NormalWorld = float3(0.0f, 0.0f, 1.0f);
		props.Roughness = 1.0f;
		props.Emissive = float3(0.0f, 0.0f, 0.0f);
		props.Metallic = 0.0f;
		props.AmbientOcclusion = 1.0f;
		return props;
	}


	float3 SampleBaseColor(float2 UV)
	{
		return TextureBaseColor.SampleLevel(SamplerLinearWrap, UV, 0.0f).xyz;
	}

	float3 SampleNormalTangent(float2 UV)
	{
		return float3(0.0f, 0.0f, 1.0f);
	}

	float SampleRoughness(float2 UV)
	{
		return 1.0f;
	}

	float3 SampleEmissive(float2 UV)
	{
		return float3(0.0f, 0.0f, 0.0f);
	}

	float SampleMetalic(float2 UV)
	{
		return 0.0f;
	}

	float SampleAmbientOcclusion(float2 UV)
	{
		return 1.0f;
	}

	Material::Properties SampleMaterial(float2 UV)
	{
		Material::Properties MatProps = MakeDefaultProperties();

		MatProps.BaseColor = SampleBaseColor(UV);
		MatProps.NormalTangent = SampleNormalTangent(UV);
		MatProps.NormalWorld = MatProps.NormalTangent;  // TODO: Transform to world space
		MatProps.Roughness = SampleRoughness(UV);
		MatProps.Emissive = SampleEmissive(UV);
		MatProps.Metallic = SampleMetalic(UV);
		MatProps.AmbientOcclusion = SampleAmbientOcclusion(UV);

		return MatProps;
	}
}  // namespace Material