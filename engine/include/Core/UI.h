#pragma once

// Minimal UI abstraction wrapping Dear ImGui.

class UI
{
public:
    void Initialize();
    void Shutdown();

    // Forward platform messages (WndProc)
    bool OnWindowMessage(void* hwnd, unsigned int msg, unsigned long long wParam, long long lParam);

    // Per-frame entry/exit
    void BeginFrame(float deltaSeconds, float displayWidth, float displayHeight);
    void BuildFPSOverlay();
    void Render();
};

extern UI GUI;
