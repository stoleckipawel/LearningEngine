#pragma once

#if USE_GUI
#include <Windows.h>

// UI manages ImGui integration (Win32 + DX12 backends).
// Threading: All ImGui calls must be made from the main thread where the
// device/command list are used. This class is not thread-safe.
class UI
{
  public:
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

  private:
	// Begins an ImGui frame. Updates delta time and display size.
	void NewFrame();

	void BuildFPSOverlay();

	// Builds UI content and finalizes draw data.
	void Build();

	void SetupDPIScaling() noexcept;
};

// Global UI instance used by the engine.
extern UI GUI;
#endif
