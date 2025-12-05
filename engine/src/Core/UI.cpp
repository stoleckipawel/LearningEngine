#include "Core/PCH.h"
#include "Core/UI.h"
#include "Core/Window.h"
#include "D3D12/RHI.h"
#include "D3D12/DescriptorHeapManager.h"

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>

// Forward declaration to ensure the Win32 backend handler is visible to this unit
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global UI instance used by the engine
UI GUI;

bool UI::OnWindowMessage(HWND hwnd, unsigned int msg, unsigned long long wParam, long long lParam)
{
    // Forward Win32 messages to ImGui; returns true if handled
    return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
}

void UI::Initialize()
{
    // Create ImGui context and set a default style
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Initialize platform backend with the window handle
    ImGui_ImplWin32_Init(GWindow.WindowHWND);

    // Initialize the DX12 backend 
    ImGui_ImplDX12_Init(
        GRHI.Device.Get(),
        NumFramesInFlight,
        GSwapChain.GetBackBufferFormat(),
        GDescriptorHeapManager.GetCBVSRVUAVHeap().GetRaw(),
        GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCPUHandle(0, DescriptorType::UI),
        GDescriptorHeapManager.GetCBVSRVUAVHeap().GetGPUHandle(0, DescriptorType::UI));
}

void UI::BeginFrame(float deltaSeconds)
{
    /*
    // Update timing and display size for this frame
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaSeconds;
    io.DisplaySize = ImVec2(GWindow.GetWidth(), GWindow.GetHeight());

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
    */
}

void UI::BuildFPSOverlay()
{
    // Simple FPS overlay anchored to top-right
    ImGuiIO& io = ImGui::GetIO();
    const float panelWidth = 220.0f;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - panelWidth, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, 100.0f), ImGuiCond_Always);
    ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Frame: %.2f ms", 1000.0f / io.Framerate);
    ImGui::End();

    // Finalize draw data for this frame
    ImGui::Render();
}

void UI::Build()
{
    //BuildFPSOverlay();
}

void UI::Render()
{
    // Ensure the shader-visible CBV/SRV/UAV heap is bound before rendering ImGui
    //ID3D12GraphicsCommandList* cmd = GRHI.GetCommandList().Get();
    //ID3D12DescriptorHeap* heaps[] = { GDescriptorHeapManager.GetCBVSRVUAVHeap().GetRaw() };
    //cmd->SetDescriptorHeaps(1, heaps);

    //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void UI::Shutdown()
{
    // Shutdown backends and destroy the ImGui context
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}