// ============================================================================
// SceneSection.h
// ----------------------------------------------------------------------------
// UI section for scene primitive spawning configuration.
//
// USAGE:
//   SceneSection sceneUI;
//   sceneUI.BuildUI();  // Called each frame in ImGui context
//
// DESIGN:
//   - Implements UIRendererSection interface for panel integration
//   - Directly modifies GScene when user changes configuration
//   - Local copies of shape/count for ImGui interaction
//
// NOTES:
//   - Changes trigger Scene::SetPrimitives() immediately
// ============================================================================

#pragma once

#include <cstdint>

#include "Framework/UIRendererSection.h"

class SceneSection final : public UIRendererSection
{
  public:
	SceneSection() noexcept;
	~SceneSection() = default;

	SceneSection(const SceneSection&) = delete;
	SceneSection(SceneSection&&) = delete;
	SceneSection& operator=(const SceneSection&) = delete;
	SceneSection& operator=(SceneSection&&) = delete;

	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Scene; }
	const char* GetTitle() const noexcept override { return "Scene"; }

	void BuildUI() override;

  private:
	// Local copies for ImGui interaction; pushed to Scene on change
	int m_shapeIndex = 0;
	int m_count = 128;
};
