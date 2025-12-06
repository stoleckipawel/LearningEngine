#pragma once

#include "SwapChain.h"

// RHI (Rendering Hardware Interface) manages Direct3D 12 device, command queues, fences, and synchronization
class RHI
{
public:
	// Initialize the RHI and all required resources
	void Initialize(bool RequireDXRSupport = false);
	// Reset all resources and shutdown RHI
	void Shutdown();	
	// Execute the current command list
	void ExecuteCommandList();
	// Set a resource barrier for a resource state transition
	void SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	// Wait for the GPU to finish executing commands
	void WaitForGPU();
	// Signal the fence for synchronization
	void Signal();
	// Flush the command queue (signal and wait)
	void Flush();

	// Use swap chain's frame-in-flight index for per-frame resources
	
	void CloseCommandListScene();	
	void CloseCommandListUI();
	void ResetCommandAllocatorScene();
	void ResetCommandListScene();

	// Checks for Shader Model 6.0 support
	void CheckShaderModel6Support();
private:
	void SelectAdapter();
	// Initialization helpers (called by Initialize)
	void CreateFactory();
	void CreateDevice(bool requireDXRSupport);
	void CreateCommandQueue();
	void CreateCommandAllocators();
	void CreateCommandLists();
	void CreateFenceAndEvent();
public:
	Microsoft::WRL::ComPtr<IDXGIFactory7> GetDxgiFactory() const { return m_DxgiFactory; }
	Microsoft::WRL::ComPtr<IDXGIAdapter1> GetAdapter() const { return m_Adapter; }
	Microsoft::WRL::ComPtr<ID3D12Device10> GetDevice() const { return m_Device; }
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_CmdQueue; }
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocatorScene(UINT frameIndex) const { return m_CmdAllocatorScene[frameIndex]; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GetCommandListScene() const { return m_CmdListScene; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GetCommandListUI() const { return m_CmdListUI; }
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocatorUI(UINT frameIndex) const { return m_CmdAllocatorUI[frameIndex]; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GetUICommandList() const { return m_CmdListUI; }
	Microsoft::WRL::ComPtr<ID3D12Fence1> GetFence() const { return m_Fence; }
	UINT64 GetFenceValueForFrame(UINT frameIndex) const { return m_FenceValues[frameIndex]; }
	void SetFenceValueForFrame(UINT frameIndex, UINT64 value) { m_FenceValues[frameIndex] = value; }
	HANDLE GetFenceEvent() const { return m_FenceEvent; }
	UINT64 GetNextFenceValue() const { return m_NextFenceValue; }
	void SetNextFenceValue(UINT64 value) { m_NextFenceValue = value; }
private:
	Microsoft::WRL::ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> m_Adapter = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device10> m_Device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdAllocatorScene[NumFramesInFlight] = {};
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CmdListScene = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdAllocatorUI[NumFramesInFlight] = {};
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CmdListUI = nullptr;

	// Fence values for each frame in flight
	UINT64 m_FenceValues[NumFramesInFlight] = { 0 };
	// Next fence value to signal
	UINT64 m_NextFenceValue = 1;
	// Fence for GPU/CPU synchronization
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence = nullptr;
	// Event handle for fence completion
	HANDLE m_FenceEvent = nullptr;
	enum D3D_FEATURE_LEVEL m_DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
};


// Global RHI instance
extern RHI GRHI;
