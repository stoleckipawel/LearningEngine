#include "RHI.h"
#include "DebugLayer.h"
#include "Window.h"


FRHI GRHI;

ComPointer<ID3D12GraphicsCommandList7> FRHI::GetCurrentCommandList()
{
	return GRHI.CmdList[GSwapChain.GetCurrentBackBufferIndex()];
}

ComPointer<ID3D12CommandAllocator> FRHI::GetCurrentCommandAllocator()
{
	return GRHI.CmdAllocator[GSwapChain.GetCurrentBackBufferIndex()];
}

UINT64 FRHI::GetCurrentFenceValue()
{
	return GRHI.FenceValues[GSwapChain.GetCurrentBackBufferIndex()];
}

void FRHI::SelectAdapter()
{
	bool bHighPerformancePreference = true;
	enum DXGI_GPU_PREFERENCE GpuPreference = bHighPerformancePreference ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

	// Try to find an adapter that meets the required GPU preference.
	for(UINT AdapterIndex = 0; SUCCEEDED(GRHI.DxgiFactory->EnumAdapterByGpuPreference(AdapterIndex, GpuPreference, IID_PPV_ARGS(&GRHI.Adapter)));++AdapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		GRHI.Adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Reject software adapters.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(GRHI.Adapter, DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	// If the above failed, fall back to the default adapter.
    if(GRHI.Adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(GRHI.DxgiFactory->EnumAdapters1(adapterIndex, &GRHI.Adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            GRHI.Adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
				// Reject software adapters.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(GRHI.Adapter.Get(), DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }	
}

bool FRHI::Initialize(bool RequireDXRSupport)
{
	GDebugLayer.Initialize();
	
	//Create DXGI Factory
	{
#if defined(_DEBUG)
		UINT dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
		UINT dxgiFactoryFlags = 0;
#endif
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&GRHI.DxgiFactory)), "RHI: Failed To Create Factory");
	}

	//Create Device
	{
		SelectAdapter();
		ThrowIfFailed(D3D12CreateDevice(GRHI.Adapter.Get(), DesiredD3DFeatureLevel, IID_PPV_ARGS(&GRHI.Device)), "RHI: Failed To Create Device");
	}
	
	//Create Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;
		ThrowIfFailed(GRHI.Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&GRHI.CmdQueue)), "RHI: Failed To Create Command Queue");
	}

	GSwapChain.Initialize();
	
	//Create Command Allocators (Frame Buffered)
	{
		for (size_t i = 0; i < BufferingCount; ++i) 
		{ 
			ThrowIfFailed(GRHI.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&GRHI.CmdAllocator[i])), "RHI: Failed To Create Command Allocator"); 
		}
	}
	
	//Create Fence
	{
		ThrowIfFailed(GRHI.Device->CreateFence(GRHI.GetCurrentFenceValue(), D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GRHI.Fence)), "RHI: Failed To Create Fence");
		GRHI.FenceValues[GSwapChain.GetCurrentBackBufferIndex()]++;
	
		FenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (!FenceEvent)
		{
			std::string message = "RHI: Failed To Create Fence Event";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}

	return true;
}

void FRHI::Shutdown()
{
	for(size_t i = 0; i < BufferingCount; ++i) 
	{ 
		GRHI.CmdAllocator[i].Release();
	}

	if (FenceEvent)
	{
		CloseHandle(FenceEvent);
	}

	Fence.Release();
	GRHI.CmdQueue.Release();
	GRHI.Device.Release();
	GRHI.DxgiFactory.Release();
}

void FRHI::ExecuteCommandList()
{
	ID3D12CommandList* ppcommandLists[] = { GRHI.GetCurrentCommandList().Get() };
	GRHI.CmdQueue->ExecuteCommandLists(1, ppcommandLists);
}

void FRHI::SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = Resource;
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = StateBefore;
	barrier.Transition.StateAfter = StateAfter;

	GRHI.GetCurrentCommandList()->ResourceBarrier(1, &barrier);
}
