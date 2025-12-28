#pragma once

#include "Framework/UIRendererSection.h"

// TimeControls exposes time scale and pause/resume controls.
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
