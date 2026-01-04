#pragma once

#include "Framework/UIRendererSection.h"

// UI section for camera and depth buffer settings.
// Exposes FOV, near/far planes, depth convention mode, and future camera controls.
class CameraSection final : public UIRendererSection
{
  public:
	CameraSection() noexcept = default;
	~CameraSection() override = default;

	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Camera; }
	const char* GetTitle() const noexcept override { return "Camera"; }

	void BuildUI() override;
};
