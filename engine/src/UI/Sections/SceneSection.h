#pragma once

#include <cstdint>

#include "Framework/UIRendererSection.h"

// SceneSection provides UI controls for scene primitive spawning.
// Directly modifies the Scene when user changes configuration.
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
