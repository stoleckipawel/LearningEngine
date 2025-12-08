
#include "PCH.h"
#include "UI.h"
#include "Window.h"
#include "D3D12/RHI.h"
#include "D3D12/DescriptorHeapManager.h"
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>

// Global UI instance used by the engine
UI GUI;

// Forward declaration to ensure the Win32 backend handler is visible to this unit.
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void AllocSRV(ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
{
    GDescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, *out_cpu_handle, *out_gpu_handle);
}

static void FreeSRV(ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
{
    GDescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cpu_handle, gpu_handle);
}

// Forwards Win32 messages to ImGui; returns true if handled.
bool UI::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(wnd, msg, wParam, lParam);
}

// Creates ImGui context and initializes Win32/DX12 backends.
void UI::Initialize()
{
    // Create ImGui context and set a default style.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable keyboard controls.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable gamepad controls.

    ImGui::StyleColorsDark();

    // Initialize platform backend with the window handle.
    ImGui_ImplWin32_Init(GWindow.WindowHWND);

 
    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = GRHI.GetDevice().Get();
    init_info.CommandQueue = GRHI.GetCommandQueue().Get();
    init_info.NumFramesInFlight = NumFramesInFlight;
    init_info.RTVFormat = GSwapChain.GetBackBufferFormat();
    init_info.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    init_info.SrvDescriptorHeap = GDescriptorHeapManager.GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetRaw();
    init_info.SrvDescriptorAllocFn = &AllocSRV;
    init_info.SrvDescriptorFreeFn = &FreeSRV;
    ImGui_ImplDX12_Init(&init_info);

    io.Fonts->AddFontDefault();        

    // Setup DPI scaling and style sizes.
    SetupDPIScaling();
}

// Begins an ImGui frame. Updates delta time and display size; binds heaps.
void UI::NewFrame(float deltaSeconds)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaSeconds > 0.0f ? deltaSeconds : io.DeltaTime; // Preserve previous if not provided.
    io.DisplaySize = ImVec2(GWindow.GetWidth(), GWindow.GetHeight());

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

// Builds a simple FPS overlay anchored to the top-right.
void UI::BuildFPSOverlay()
{
    ImGuiIO& io = ImGui::GetIO();
    const float panelWidth = 220.0f;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - panelWidth, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, 100.0f), ImGuiCond_Always);
    ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Frame: %.2f ms", 1000.0f / io.Framerate);
    ImGui::End();
}

// Builds demo UI and finalizes draw data for this frame.
void UI::Build()
{
    //BuildFPSOverlay();

    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);

    ImGui::Render();    
}

void UI::Update(float deltaSeconds)
{
    NewFrame(deltaSeconds);
    Build();
}

// Submits ImGui draw data using the current DX12 command list.
void UI::Render()
{
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRHI.GetCommandList().Get());
}

// Shuts down ImGui backends and destroys the context.
void UI::Shutdown()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// Configures DPI awareness and scales style/font sizes accordingly.
void UI::SetupDPIScaling()
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float mainScale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
    ImGuiStyle& style = ImGui::GetStyle();
    style.FontSizeBase = 16.0f;
    // Bake a fixed style scale until dynamic style scaling is supported.
    style.ScaleAllSizes(mainScale);
}