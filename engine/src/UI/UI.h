#pragma once

#include <Windows.h>

#include <memory>

#include "Sections/ViewMode.h"

class RendererPanel;
class UIRendererSection;
class StatsOverlay;

// UI manages ImGui integration (Win32 + DX12 backends).
// Threading: All ImGui calls must be made from the main thread where the
// device/command list are used. This class is not thread-safe.
class UI
{
  public:
	UI() = default;
	~UI() noexcept;

	// Creates ImGui context and initializes Win32/DX12 backends.
	// May log/abort on failure via engine error helpers.
	void Initialize();

	// Shuts down backends and destroys ImGui context.
	void Shutdown() noexcept;

	// Forwards Win32 messages to ImGui.
	bool OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	// Update UI state and build draw lists for the current frame.
	void Update();

	// Submits ImGui draw data to the current DX12 command list.
	void Render() noexcept;

	ViewMode::Type GetViewMode() noexcept;

  private:
	// Begins an ImGui frame. Updates delta time and display size.
	void NewFrame();

	// Builds UI content and finalizes draw data.
	void Build();

	void SetupDPIScaling() noexcept;

	std::unique_ptr<RendererPanel> m_rendererPanel;
};

// Global UI instance used by the engine.
extern UI GUI;
