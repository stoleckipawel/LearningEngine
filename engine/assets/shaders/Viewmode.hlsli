#pragma once

// Shader-side view mode mapping.
// Keep these values in sync with ViewMode::Type in engine/src/UI/Sections/ViewMode.h

#include "ConstantBufferData.hlsli"
#include "Material.hlsli"

namespace ViewMode
{
	// Keep these values in sync with ViewMode::Type in engine/src/UI/Sections/ViewMode.h
	static const uint Lit = 0u;
	static const uint GBufferDiffuse = 1u;
	static const uint GBufferNormal = 2u;
	static const uint GBufferRoughness = 3u;
	static const uint GBufferMetallic = 4u;
	static const uint GBufferEmissive = 5u;
	static const uint GBufferAmbientOcclusion = 6u;
	static const uint DirectDiffuse = 7u;
	static const uint DirectSpecular = 8u;
	static const uint IndirectDiffuse = 9u;
	static const uint IndirectSpecular = 10u;

	float3 Resolve(
	    float3 litColor,
	    Material::Properties matProps,
	    float3 directDiffuse,
	    float3 directSpecular,
	    float3 indirectDiffuse,
	    float3 indirectSpecular)
	{
		switch (ViewModeIndex)
		{
			case Lit:
				return litColor;
			case GBufferDiffuse:
				return matProps.BaseColor;
			case GBufferNormal:
				return matProps.NormalWorld * 0.5f + 0.5f;
			case GBufferRoughness:
				return matProps.Roughness.xxx;
			case GBufferMetallic:
				return matProps.Metallic.xxx;
			case GBufferEmissive:
				return matProps.Emissive;
			case GBufferAmbientOcclusion:
				return matProps.AmbientOcclusion.xxx;
			case DirectDiffuse:
				return directDiffuse;
			case DirectSpecular:
				return directSpecular;
			case IndirectDiffuse:
				return indirectDiffuse;
			case IndirectSpecular:
				return indirectSpecular;
		}

		return litColor;
	}
}  // namespace ViewMode
