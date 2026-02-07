// ============================================================================
// BasicShapesLevel.h — Procedural primitive cluster (default starter level)
// ============================================================================

#pragma once

#include "Level/Level.h"

class BasicShapesLevel final : public Level
{
  public:
	std::string_view GetName() const override { return "BasicShapes"; }

	std::string_view GetDescription() const override { return "Procedural primitives cluster — engine default starter level"; }

	LevelDesc BuildDescription() const override
	{
		LevelDesc desc;
		MeshRequest req;
		req.source = AssetSource::Procedural;
		req.procedural = PrimitiveRequest{MeshFactory::Shape::Box, 500, {0.0f, 0.0f, 50.0f}, {100.0f, 100.0f, 100.0f}, 1337};
		desc.meshRequests.push_back(req);
		return desc;
	}
};
