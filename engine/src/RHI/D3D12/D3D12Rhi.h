#pragma once

#include "D3D12SwapChain.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// RHI (Rendering Hardware Interface) manages Direct3D 12 device, command
// queues, fences, and synchronization. API is designed to be lightweight and
// avoid unnecessary COM refcount churn: getters return const references to
// internal ComPtr members where callers only need temporary access.
class D3D12Rhi
{
public:
	// Initialize the RHI and all required resources
	void Initialize(bool RequireDXRSupport = false) noexcept;

	// Reset all resources and shutdown RHI
	void Shutdown() noexcept;

	// Execute the current command list
	void ExecuteCommandList() noexcept;

	// Set a resource barrier for a resource state transition
	void SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) noexcept;

	// Wait for the GPU to finish executing commands
	void WaitForGPU() noexcept;

	// Signal the fence for synchronization
	void Signal() noexcept;

	// Flush the command queue (signal and wait)
	void Flush() noexcept;

	void CloseCommandListScene() noexcept;
	void ResetCommandAllocator() noexcept;
	void ResetCommandList() noexcept;

	void CheckShaderModel6Support() const noexcept;

private:
	void SelectAdapter() noexcept;
	void CreateFactory();
	void CreateDevice(bool requireDXRSupport);
	void CreateCommandQueue();
	void CreateCommandAllocators();
	void CreateCommandLists();
	void CreateFenceAndEvent();

public:
	// Return const references to ComPtr members to avoid extra AddRef/Release
	// when callers only need transient access. Callers that need ownership can
	// copy the ComPtr explicitly.
	const ComPtr<IDXGIFactory7>& GetDxgiFactory() const noexcept { return m_DxgiFactory; }
	const ComPtr<IDXGIAdapter1>& GetAdapter() const noexcept { return m_Adapter; }
	const ComPtr<ID3D12Device10>& GetDevice() const noexcept { return m_Device; }
	const ComPtr<ID3D12CommandQueue>& GetCommandQueue() const noexcept { return m_CmdQueue; }
	const ComPtr<ID3D12CommandAllocator>& GetCommandAllocatorScene(UINT FrameInFlightIndex) const noexcept { return m_CmdAllocatorScene[FrameInFlightIndex]; }
	const ComPtr<ID3D12GraphicsCommandList7>& GetCommandList() const noexcept { return m_CmdListScene; }
	const ComPtr<ID3D12Fence1>& GetFence() const noexcept { return m_Fence; }

	UINT64 GetFenceValueForFrame(UINT FrameInFlightIndex) const noexcept { return m_FenceValues[FrameInFlightIndex]; }
	void SetFenceValueForFrame(UINT FrameInFlightIndex, UINT64 value) noexcept { m_FenceValues[FrameInFlightIndex] = value; }
	HANDLE GetFenceEvent() const noexcept { return m_FenceEvent; }
	UINT64 GetNextFenceValue() const noexcept { return m_NextFenceValue; }
	void SetNextFenceValue(UINT64 value) noexcept { m_NextFenceValue = value; }

private:
	ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;
	ComPtr<IDXGIAdapter1> m_Adapter = nullptr;
	ComPtr<ID3D12Device10> m_Device = nullptr;
	ComPtr<ID3D12CommandQueue> m_CmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_CmdAllocatorScene[EngineSettings::FramesInFlight] = {};
	ComPtr<ID3D12GraphicsCommandList7> m_CmdListScene = nullptr;

	// Fence values for each frame in flight
	UINT64 m_FenceValues[EngineSettings::FramesInFlight] = { 0 };
	// Next fence value to signal
	UINT64 m_NextFenceValue = 1;
	// Fence for GPU/CPU synchronization
	ComPtr<ID3D12Fence1> m_Fence = nullptr;
	// Event handle for fence completion
	HANDLE m_FenceEvent = nullptr;
	D3D_FEATURE_LEVEL m_DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
};


// Global D3D12 RHI instance
extern D3D12Rhi GD3D12Rhi;

