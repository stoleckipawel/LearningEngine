#include "Core/PCH.h"
#include "D3D12/RHI.h"
#include "D3D12/DebugLayer.h"
#include "Core/Window.h"

RHI GRHI;

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

// Checks for Shader Model 6.0 support
void RHI::CheckShaderModel6Support() const
{
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
	if (GRHI.Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)
	{
		LogMessage("RHI: Device does not support Shader Model 6.0. Minimum required for engine.", ELogType::Fatal);
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

	CheckShaderModel6Support();	
    
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
			ThrowIfFailed(GRHI.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&GRHI.CmdAllocatorScene[i])), "RHI: Failed To Create Scene Command Allocator"); 
			ThrowIfFailed(GRHI.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&GRHI.CmdAllocatorUI[i])), "RHI: Failed To Create UI Command Allocator");
		}
	}

	// Create Command Lists (Frame Buffered)
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		ThrowIfFailed(GRHI.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, GRHI.CmdAllocatorScene[i].Get(), nullptr, IID_PPV_ARGS(&GRHI.CmdListScene[i])), "RHI: Failed To Create Scene Command List");
		ThrowIfFailed(GRHI.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, GRHI.CmdAllocatorUI[i].Get(), nullptr, IID_PPV_ARGS(&GRHI.CmdListUI[i])), "RHI: Failed To Create UI Command List");
	}
    
	// Create Fence for GPU/CPU synchronization
	{
		for (UINT i = 0; i < NumFramesInFlight; ++i)
		{
			GRHI.FenceValues[i] = 0;
		}

		ThrowIfFailed(GRHI.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GRHI.Fence)), "RHI: Failed To Create Fence");
    
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!FenceEvent)
		{
			LogMessage("RHI: Failed To Create Fence Event", ELogType::Fatal);
		}
	}
}

ComPointer<ID3D12GraphicsCommandList7> RHI::GetCommandListScene()
{
	return GRHI.CmdListScene[GSwapChain.GetBackBufferIndex()];
}

ComPointer<ID3D12GraphicsCommandList7> RHI::GetCommandListUI()
{
	return GRHI.CmdListUI[GSwapChain.GetBackBufferIndex()];
}

ComPointer<ID3D12CommandAllocator> RHI::GetCommandAllocatorScene()
{
	return GRHI.CmdAllocatorScene[GSwapChain.GetBackBufferIndex()];
}

ComPointer<ID3D12CommandAllocator> RHI::GetCommandAllocatorUI()
{
	return GRHI.CmdAllocatorUI[GSwapChain.GetBackBufferIndex()];
}

UINT64 RHI::GetFenceValue()
{
	return GRHI.FenceValues[GSwapChain.GetBackBufferIndex()];
}

void RHI::CloseCommandListScene(UINT FrameIndex)
{
	ThrowIfFailed(GRHI.CmdListScene[FrameIndex]->Close(), "RHI: Failed To Close Scene Command List");
}

void RHI::CloseCommandListUI(UINT FrameIndex)
{
	ThrowIfFailed(GRHI.CmdListUI[FrameIndex]->Close(), "RHI: Failed To Close UI Command List");		
}

void RHI::CloseCommandListScene()
{
	CloseCommandListScene(GSwapChain.GetBackBufferIndex());
}

void RHI::CloseCommandListUI()
{
	CloseCommandListUI(GSwapChain.GetBackBufferIndex());
}

void RHI::CloseCommandLists()
{
	for (UINT i = 0; i < NumFramesInFlight; ++i)
	{
		CloseCommandListScene(i);
		CloseCommandListUI(i);
	}
}

void RHI::ResetCommandLists()
{
	ThrowIfFailed(GetCommandAllocatorScene()->Reset(), "RHI: Failed To Reset Scene Command Allocator");
	ThrowIfFailed(GetCommandListScene()->Reset(GetCommandAllocatorScene().Get(), nullptr), "RHI: Failed To Reset Scene Command List");
}

// Releases all resources and shuts down the RHI
void RHI::Shutdown()
{
	for(UINT i = 0; i < NumFramesInFlight; ++i) 
	{ 
		GRHI.CmdAllocatorScene[i].Release();
		GRHI.CmdListScene[i].Release();
		GRHI.CmdAllocatorUI[i].Release();
		GRHI.CmdListUI[i].Release();
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
	ID3D12CommandList* ppcommandLists[] = { GRHI.GetCommandListScene().Get() };
	GRHI.CmdQueue->ExecuteCommandLists(1, ppcommandLists);
}


// Sets a resource barrier for a resource state transition
void RHI::SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = Resource;
	// Transition all subresources (depth + stencil planes)
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = StateBefore;
	barrier.Transition.StateAfter = StateAfter;

	GRHI.GetCommandListScene()->ResourceBarrier(1, &barrier);
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
