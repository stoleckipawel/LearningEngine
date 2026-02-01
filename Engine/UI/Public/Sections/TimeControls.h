// ============================================================================
// TimeControls.h
// ----------------------------------------------------------------------------
// UI section for time scale and pause/resume controls.
//
// USAGE:
//   TimeControls timeCtrl(timer);
//   timeCtrl.BuildUI();  // Called each frame in ImGui context
//
// DESIGN:
//   - Implements UIRendererSection interface for panel integration
//   - Exposes Timer scale and pause state
//   - Slider for time scale, checkbox for pause/resume
//
// NOTES:
//   - Timer reference must be provided at construction
// ============================================================================

#pragma once

#include "Framework/UIRendererSection.h"

class Timer;

class TimeControls final : public UIRendererSection
{
  public:
	explicit TimeControls(Timer& timer) noexcept;
	~TimeControls() = default;

	TimeControls(const TimeControls&) = delete;
	TimeControls(TimeControls&&) = delete;
	TimeControls& operator=(const TimeControls&) = delete;
	TimeControls& operator=(TimeControls&&) = delete;

	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Time; }
	const char* GetTitle() const noexcept override { return "Time"; }

	void BuildUI() override;

  private:
	Timer& m_timer;
};
