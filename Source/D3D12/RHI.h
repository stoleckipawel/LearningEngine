#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "SwapChain.h"

class RHI
{
public:
	bool Initialize(bool RequireDXRSupport = false);
	void SelectAdapter();
	void Shutdown();
	void ExecuteCommandList();
	void SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	void WaitForGPU();
	void Signal();
	void Flush();

	ComPointer<ID3D12GraphicsCommandList7> GetCurrentCommandList();
	ComPointer<ID3D12CommandAllocator> GetCurrentCommandAllocator();
	UINT64 GetCurrentFenceValue();
public:
	ComPointer<IDXGIFactory7> DxgiFactory = nullptr;
	ComPointer<IDXGIAdapter1> Adapter = nullptr;
	ComPointer<ID3D12Device10> Device = nullptr;
	ComPointer<ID3D12CommandQueue> CmdQueue = nullptr;
	ComPointer<ID3D12CommandAllocator> CmdAllocator[NumFramesInFlight] = {};
	ComPointer<ID3D12GraphicsCommandList7> CmdList[NumFramesInFlight] = {};

	UINT64 FenceValues[NumFramesInFlight] = { 0 };
	UINT64 NextFenceValue = 1;
	ComPointer<ID3D12Fence1> Fence = nullptr;
	HANDLE FenceEvent = nullptr;

	enum D3D_FEATURE_LEVEL DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
public:
};

extern RHI GRHI;
