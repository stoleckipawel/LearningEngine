#include "Core/PCH.h"
#include "Core/UI.h"
#include "Core/Window.h"

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>

// Forward declaration to ensure the Win32 backend handler is visible to this unit
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "D3D12/RHI.h"
#include "D3D12/DescriptorHeapManager.h"

// Global UI instance used by the engine
UI GUI;

void UI::Initialize()
{
    // Create ImGui context and set a default style
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Initialize platform backend with the window handle
    ImGui_ImplWin32_Init(GWindow.WindowHWND);

    // Reserve descriptor heap space for ImGui font atlas and related SRVs
    D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCPUHandle(0, DescriptorType::UI);
    D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = GDescriptorHeapManager.GetCBVSRVUAVHeap().GetGPUHandle(0, DescriptorType::UI);

    // Initialize the DX12 backend 
    ImGui_ImplDX12_Init(
        GRHI.Device.Get(),
        NumFramesInFlight,
        GSwapChain.GetBackBufferFormat(),
        GDescriptorHeapManager.GetCBVSRVUAVHeap().GetRaw(),
        cpuStart,
        gpuStart);

    // Ensure device objects (e.g., font atlas) are created up front
    ImGui_ImplDX12_CreateDeviceObjects();
}

void UI::Shutdown()
{
    // Shutdown backends and destroy the ImGui context
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool UI::OnWindowMessage(void* hwnd, unsigned int msg, unsigned long long wParam, long long lParam)
{
    // Forward Win32 messages to ImGui; returns true if handled
    return ImGui_ImplWin32_WndProcHandler(reinterpret_cast<HWND>(hwnd), msg, wParam, lParam);
}

void UI::BeginFrame(float deltaSeconds)
{
    // Update timing and display size for this frame
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaSeconds;
    const D3D12_VIEWPORT vp = GSwapChain.GetDefaultViewport();
    io.DisplaySize = ImVec2(vp.Width, vp.Height);

    // Start a new frame for both backends and ImGui core
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
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
    // Build all UI panels for this frame
    BuildFPSOverlay();
}

void UI::Render()
{
    // Bind the shared CBV/SRV/UAV heap required by ImGui's DX12 backend
    //ID3D12DescriptorHeap* heaps[] = { GDescriptorHeapManager.GetCBVSRVUAVHeap().GetRaw() };
    //GRHI.GetCommandList()->SetDescriptorHeaps(1, heaps);

    // Submit ImGui draw data using the engine's command list
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRHI.GetCommandList().Get());
}
