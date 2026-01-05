// ============================================================================
// StatsOverlay.h
// ----------------------------------------------------------------------------
// UI section displaying performance statistics (FPS, frame time, frame index).
//
// USAGE:
//   StatsOverlay stats;
//   stats.BuildUI();  // Called each frame in ImGui context
//
// DESIGN:
//   - Implements UIRendererSection interface for panel integration
//   - Renders only contents; caller owns window placement/layout
//   - Queries Engine::Timer for timing data
//
// NOTES:
//   - Lightweight: no state beyond base class
// ============================================================================

#pragma once

#include "Framework/UIRendererSection.h"

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
