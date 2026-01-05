// ============================================================================
// TimeControls.h
// ----------------------------------------------------------------------------
// UI section for time scale and pause/resume controls.
//
// USAGE:
//   TimeControls timeCtrl;
//   timeCtrl.BuildUI();  // Called each frame in ImGui context
//
// DESIGN:
//   - Implements UIRendererSection interface for panel integration
//   - Exposes Engine::Timer scale and pause state
//   - Slider for time scale, button for pause/resume
//
// NOTES:
//   - Changes take effect immediately via Engine::GTimer
// ============================================================================

#pragma once

#include "Framework/UIRendererSection.h"

class TimeControls final : public UIRendererSection
{
  public:
	TimeControls() noexcept = default;
	~TimeControls() = default;

	TimeControls(const TimeControls&) = delete;
	TimeControls(TimeControls&&) = delete;
	TimeControls& operator=(const TimeControls&) = delete;
	TimeControls& operator=(TimeControls&&) = delete;

	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Time; }
	const char* GetTitle() const noexcept override { return "Time"; }

	void BuildUI() override;
};
