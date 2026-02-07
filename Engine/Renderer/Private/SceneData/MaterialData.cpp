// =============================================================================
// MaterialData.cpp
// =============================================================================

#include "PCH.h"
#include "Renderer/Public/SceneData/MaterialData.h"
#include "Assets/MaterialDesc.h"

MaterialData MaterialData::FromDesc(const MaterialDesc& desc)
{
	MaterialData mat = {};
	mat.baseColor = desc.baseColor;
	mat.metallic = desc.metallic;
	mat.roughness = desc.roughness;
	mat.f0 = desc.f0;
	// Texture binding deferred until TextureManager supports dynamic loading
	return mat;
}
