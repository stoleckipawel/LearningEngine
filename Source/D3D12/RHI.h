#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "SwapChain.h"


// RHI (Rendering Hardware Interface) manages Direct3D 12 device, command queues, fences, and synchronization
class RHI
{
public:
	// Initialize the RHI and all required resources
	void Initialize(bool RequireDXRSupport = false);
	// Close all command lists
	void CloseCommandLists();
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

	ComPointer<ID3D12GraphicsCommandList7> GetCommandList();
	ComPointer<ID3D12CommandAllocator> GetCommandAllocator();
	UINT64 GetFenceValue();
private:
	void SelectAdapter();
public:
	ComPointer<IDXGIFactory7> DxgiFactory = nullptr;
	ComPointer<IDXGIAdapter1> Adapter = nullptr;
	ComPointer<ID3D12Device10> Device = nullptr;
	ComPointer<ID3D12CommandQueue> CmdQueue = nullptr;
	ComPointer<ID3D12CommandAllocator> CmdAllocator[NumFramesInFlight] = {};
	ComPointer<ID3D12GraphicsCommandList7> CmdList[NumFramesInFlight] = {};

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
