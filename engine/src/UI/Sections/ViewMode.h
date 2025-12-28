#pragma once

#include <cstdint>

#include "Framework/UIRendererSection.h"

// ViewMode holds the user-selected debug view for the renderer.
class ViewMode final : public UIRendererSection
{
  public:
	enum class Id : std::uint8_t
	{
		Lit = 0,
		GBufferDiffuse,
		GBufferNormal,
		GBufferRoughness,
		GBufferMetallic,
		GBufferEmissive,
		Count
	};

	ViewMode() noexcept = default;
	~ViewMode() = default;

	ViewMode(const ViewMode&) = delete;
	ViewMode(ViewMode&&) = delete;
	ViewMode& operator=(const ViewMode&) = delete;
	ViewMode& operator=(ViewMode&&) = delete;

	Id Get() const noexcept { return m_mode; }
	void Set(Id mode) noexcept { m_mode = mode; }


	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::ViewMode; }
	const char* GetTitle() const noexcept override { return "Renderer"; }

	// Builds the view mode controls. Renders only contents; window/layout is owned by the caller.
	void BuildUI() override;
  private:
	Id m_mode = Id::Lit;
};
