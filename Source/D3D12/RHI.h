#pragma once
#include "../Vendor/Windows/WinInclude.h"


class FRHI
{
public:
	bool Initialize(bool RequireDXRSupport = false);
	void Shutdown();
	void SignalAndWait();
	void Flush();
	void ExecuteCommandList();
	void SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

public:
	ComPointer<IDXGIFactory7> DxgiFactory = nullptr;
	ComPointer<IDXGIAdapter1> Adapter = nullptr;
	ComPointer<ID3D12Device10> Device = nullptr;
	ComPointer<ID3D12CommandQueue> CmdQueue = nullptr;
	ComPointer<ID3D12CommandAllocator> CmdAllocator = nullptr;
	ComPointer<ID3D12GraphicsCommandList7> CmdList = nullptr;

	ComPointer<ID3D12Fence1> Fence = nullptr;;
	UINT64 FenceValue = 0;
	HANDLE FenceEvent = nullptr;
public:
};

extern FRHI GRHI;
