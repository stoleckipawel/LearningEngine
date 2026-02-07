// ============================================================================
// SponzaLevel.h — Sponza Palace PBR test scene
// ============================================================================

#pragma once

#include "Level/Level.h"

class SponzaLevel final : public Level
{
  public:
	std::string_view GetName() const override { return "Sponza"; }

	std::string_view GetDescription() const override { return "Sponza Palace — standard PBR test scene"; }

	LevelDesc BuildDescription() const override
	{
		LevelDesc desc;
		MeshRequest req;
		req.source = AssetSource::Imported;
		req.assetPath = "Sponza/Sponza.gltf";
		req.assetType = AssetType::Mesh;
		desc.meshRequests.push_back(req);
		return desc;
	}
};
