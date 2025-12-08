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

	void CloseCommandListScene();	
	void CloseCommandListUI();
	void ResetCommandAllocator();
	void ResetCommandList();

	void CheckShaderModel6Support();
private:
	void SelectAdapter();
	void CreateFactory();
	void CreateDevice(bool requireDXRSupport);
	void CreateCommandQueue();
	void CreateCommandAllocators();
	void CreateCommandLists();
	void CreateFenceAndEvent();
public:
	ComPtr<IDXGIFactory7> GetDxgiFactory() const { return m_DxgiFactory; }
	ComPtr<IDXGIAdapter1> GetAdapter() const { return m_Adapter; }
	ComPtr<ID3D12Device10> GetDevice() const { return m_Device; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_CmdQueue; }
	ComPtr<ID3D12CommandAllocator> GetCommandAllocatorScene(UINT FrameInFlightIndex) const { return m_CmdAllocatorScene[FrameInFlightIndex]; }
	ComPtr<ID3D12GraphicsCommandList7> GetCommandListScene() const { return m_CmdListScene; }
	ComPtr<ID3D12GraphicsCommandList7> GetCommandListUI() const { return m_CmdListUI; }
	ComPtr<ID3D12CommandAllocator> GetCommandAllocatorUI(UINT FrameInFlightIndex) const { return m_CmdAllocatorUI[FrameInFlightIndex]; }
	ComPtr<ID3D12GraphicsCommandList7> GetUICommandList() const { return m_CmdListUI; }
	ComPtr<ID3D12Fence1> GetFence() const { return m_Fence; }
	UINT64 GetFenceValueForFrame(UINT FrameInFlightIndex) const { return m_FenceValues[FrameInFlightIndex]; }
	void SetFenceValueForFrame(UINT FrameInFlightIndex, UINT64 value) { m_FenceValues[FrameInFlightIndex] = value; }
	HANDLE GetFenceEvent() const { return m_FenceEvent; }
	UINT64 GetNextFenceValue() const { return m_NextFenceValue; }
	void SetNextFenceValue(UINT64 value) { m_NextFenceValue = value; }
private:
	ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;
	ComPtr<IDXGIAdapter1> m_Adapter = nullptr;
	ComPtr<ID3D12Device10> m_Device = nullptr;
	ComPtr<ID3D12CommandQueue> m_CmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_CmdAllocatorScene[NumFramesInFlight] = {};
	ComPtr<ID3D12GraphicsCommandList7> m_CmdListScene = nullptr;
	ComPtr<ID3D12CommandAllocator> m_CmdAllocatorUI[NumFramesInFlight] = {};
	ComPtr<ID3D12GraphicsCommandList7> m_CmdListUI = nullptr;

	// Fence values for each frame in flight
	UINT64 m_FenceValues[NumFramesInFlight] = { 0 };
	// Next fence value to signal
	UINT64 m_NextFenceValue = 1;
	// Fence for GPU/CPU synchronization
	ComPtr<ID3D12Fence1> m_Fence = nullptr;
	// Event handle for fence completion
	HANDLE m_FenceEvent = nullptr;
	enum D3D_FEATURE_LEVEL m_DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
};


// Global RHI instance
extern RHI GRHI;
