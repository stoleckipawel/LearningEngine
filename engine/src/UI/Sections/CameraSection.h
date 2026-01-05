// ============================================================================
// CameraSection.h
// ----------------------------------------------------------------------------
// UI section for camera and depth buffer settings.
//
// USAGE:
//   CameraSection cameraUI;
//   cameraUI.BuildUI();  // Called each frame in ImGui context
//
// DESIGN:
//   - Implements UIRendererSection interface for panel integration
//   - Exposes FOV, near/far planes, depth convention mode
//   - Changes applied directly to GCamera and GDepthConvention
//
// NOTES:
//   - Future: camera position/rotation controls
// ============================================================================

#pragma once

#include "Framework/UIRendererSection.h"

class CameraSection final : public UIRendererSection
{
  public:
	CameraSection() noexcept = default;
	~CameraSection() override = default;

	UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Camera; }
	const char* GetTitle() const noexcept override { return "Camera"; }

	void BuildUI() override;
};
