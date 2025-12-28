#pragma once

#include "Framework/UIRendererSection.h"


// StatsOverlay draws performance stats (FPS, frame time, frame index).
// It renders only the contents; the caller owns window placement/layout.

class StatsOverlay final : public UIRendererSection
{
  public:
	StatsOverlay() noexcept = default;
	~StatsOverlay() = default;

	StatsOverlay(const StatsOverlay&) = delete;
	StatsOverlay(StatsOverlay&&) = delete;
	StatsOverlay& operator=(const StatsOverlay&) = delete;
	StatsOverlay& operator=(StatsOverlay&&) = delete;


	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Stats; }
	const char* GetTitle() const noexcept override { return "Stats"; }

	// Builds the overlay contents. Call once per-frame while an ImGui frame is active.
	void BuildUI() override;
};
