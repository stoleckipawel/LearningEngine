#pragma once

#include "SwapChain.h"

// RHI (Rendering Hardware Interface) manages Direct3D 12 device, command queues, fences, and synchronization
class RHI
{
public:
	// Initialize the RHI and all required resources
	void Initialize(bool RequireDXRSupport = false);
	// Release all resources and shutdown RHI
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

	// Close all command lists
	void CloseCommandLists();
	void CloseCommandListScene(UINT FrameIndex);
	void CloseCommandListUI(UINT FrameIndex);
	void CloseCommandListScene();	
	void ResetCommandLists();

	ComPointer<ID3D12GraphicsCommandList7> GetCommandListScene();
	ComPointer<ID3D12GraphicsCommandList7> GetCommandListUI();
	ComPointer<ID3D12CommandAllocator> GetCommandAllocatorScene();
	ComPointer<ID3D12CommandAllocator> GetCommandAllocatorUI();

	UINT64 GetFenceValue();

	// Checks for Shader Model 6.0 support
	void CheckShaderModel6Support() const;
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
	ComPointer<IDXGIFactory7> DxgiFactory = nullptr;
	ComPointer<IDXGIAdapter1> Adapter = nullptr;
	ComPointer<ID3D12Device10> Device = nullptr;
	ComPointer<ID3D12CommandQueue> CmdQueue = nullptr;
	ComPointer<ID3D12CommandAllocator> CmdAllocatorScene[NumFramesInFlight] = {};
	ComPointer<ID3D12GraphicsCommandList7> CmdListScene[NumFramesInFlight] = {};
	ComPointer<ID3D12CommandAllocator> CmdAllocatorUI[NumFramesInFlight] = {};
	ComPointer<ID3D12GraphicsCommandList7> CmdListUI[NumFramesInFlight] = {};

	// Fence values for each frame in flight
	UINT64 FenceValues[NumFramesInFlight] = { 0 };
	// Next fence value to signal
	UINT64 NextFenceValue = 1;
	// Fence for GPU/CPU synchronization
	ComPointer<ID3D12Fence1> Fence = nullptr;
	// Event handle for fence completion
	HANDLE FenceEvent = nullptr;
private:
	enum D3D_FEATURE_LEVEL m_DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
};


// Global RHI instance
extern RHI GRHI;
