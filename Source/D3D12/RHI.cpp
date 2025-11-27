#include "PCH.h"
#include "RHI.h"
#include "DebugLayer.h"
#include "Window.h"

RHI GRHI;

ComPointer<ID3D12GraphicsCommandList7> RHI::GetCommandList()
{
	return GRHI.CmdList[GSwapChain.GetBackBufferIndex()];
}

ComPointer<ID3D12CommandAllocator> RHI::GetCommandAllocator()
{
	return GRHI.CmdAllocator[GSwapChain.GetBackBufferIndex()];
}

UINT64 RHI::GetFenceValue()
{
	return GRHI.FenceValues[GSwapChain.GetBackBufferIndex()];
}


// Selects the best available adapter (GPU) that supports Direct3D 12
void RHI::SelectAdapter()
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
		if (SUCCEEDED(D3D12CreateDevice(GRHI.Adapter, m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
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
			if (SUCCEEDED(D3D12CreateDevice(GRHI.Adapter.Get(), m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}   
}


// Initializes the RHI and all required resources
void RHI::Initialize(bool RequireDXRSupport)
{
	GDebugLayer.Initialize();
    
	// Create DXGI Factory
	{
#if defined(_DEBUG)
		UINT dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
		UINT dxgiFactoryFlags = 0;
#endif
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&GRHI.DxgiFactory)), "RHI: Failed To Create Factory");
	}

	// Create Device
	{
		SelectAdapter();
		ThrowIfFailed(D3D12CreateDevice(GRHI.Adapter.Get(), m_DesiredD3DFeatureLevel, IID_PPV_ARGS(&GRHI.Device)), "RHI: Failed To Create Device");
	}
    
	// Create Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;
		ThrowIfFailed(GRHI.Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&GRHI.CmdQueue)), "RHI: Failed To Create Command Queue");
	}
    
	// Create Command Allocators (Frame Buffered)
	{
		for (size_t i = 0; i < NumFramesInFlight; ++i) 
		{ 
			ThrowIfFailed(GRHI.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&GRHI.CmdAllocator[i])), "RHI: Failed To Create Command Allocator"); 
			ThrowIfFailed(GRHI.CmdAllocator[i]->Reset(), "RHI: Failed to Reset Command Allocator");
		}
	}

	// Create Command Lists (Frame Buffered)
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		ThrowIfFailed(GRHI.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, GRHI.CmdAllocator[i].Get(), nullptr, IID_PPV_ARGS(&GRHI.CmdList[i])), "RHI: Failed To Create Command List");
	}
    
	// Create Fence for GPU/CPU synchronization
	{
		for (size_t i = 0; i < NumFramesInFlight; ++i)
		{
			GRHI.FenceValues[i] = 0;
		}

		ThrowIfFailed(GRHI.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GRHI.Fence)), "RHI: Failed To Create Fence");
    
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!FenceEvent)
		{
			LogError("RHI: Failed To Create Fence Event", ELogType::Fatal);
		}
	}
}


// Closes all command lists for each frame in flight
void RHI::CloseCommandLists()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		ThrowIfFailed(GRHI.CmdList[i]->Close(), "RHI: Failed To Close Command List");
	}
}


// Releases all resources and shuts down the RHI
void RHI::Shutdown()
{
	for(size_t i = 0; i < NumFramesInFlight; ++i) 
	{ 
		GRHI.CmdAllocator[i].Release();
		GRHI.CmdList[i].Release();
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


// Executes the current command list on the command queue
void RHI::ExecuteCommandList()
{
	ID3D12CommandList* ppcommandLists[] = { GRHI.GetCommandList().Get() };
	GRHI.CmdQueue->ExecuteCommandLists(1, ppcommandLists);
}


// Sets a resource barrier for a resource state transition
void RHI::SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = Resource;
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = StateBefore;
	barrier.Transition.StateAfter = StateAfter;

	GRHI.GetCommandList()->ResourceBarrier(1, &barrier);
}


// Waits for the GPU to finish executing commands
void RHI::WaitForGPU()
{
	UINT FenceCurrentValue = GetFenceValue();
	UINT FenceCompletedValue = Fence->GetCompletedValue();

	if (FenceCompletedValue < FenceCurrentValue)
	{
		ThrowIfFailed(Fence->SetEventOnCompletion(FenceCurrentValue, FenceEvent), "RHI: Failed To Signal Command Queue");
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}


// Signals the fence for synchronization
void RHI::Signal()
{
	// Schedule a Signal command in the queue. -> Updates Fence Completed Value
	UINT64 currentFenceValue = NextFenceValue++;
	ThrowIfFailed(CmdQueue->Signal(Fence.Get(), currentFenceValue), "RHI: Failed To Signal Command Queue");

	// Set the fence value for the next frame.
	FenceValues[GSwapChain.GetBackBufferIndex()] = currentFenceValue;
}


// Flushes the command queue (signal and wait)
void RHI::Flush()
{
	Signal();
	WaitForGPU();
}
