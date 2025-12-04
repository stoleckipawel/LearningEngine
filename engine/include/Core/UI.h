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
    void BeginFrame(float deltaSeconds);
    void BuildFPSOverlay();
    void Build();
    void Render();
};

extern UI GUI;
