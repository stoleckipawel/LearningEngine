#pragma once

#if USE_GUI
class UI
{
public:
    // Creates ImGui context and initializes Win32/DX12 backends.
    void Initialize();
    // Shuts down backends and destroys ImGui context.
    void Shutdown();

    // Forwards Win32 messages to ImGui. Returns true if handled.
    bool OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void Update(double deltaSeconds);

    // Submits ImGui draw data to the current DX12 command list.
    void Render();
private:
    // Begins an ImGui frame. Updates delta time and display size.
    void NewFrame(double deltaSeconds);
    // Builds a simple FPS overlay in the top-right corner.
    void BuildFPSOverlay();
    // Builds demo/UI content and finalizes draw data.
    void Build();

    // Configures DPI awareness and scales style sizes appropriately.
    void SetupDPIScaling();
};

// Global UI instance used by the engine.
extern UI GUI;
#endif




