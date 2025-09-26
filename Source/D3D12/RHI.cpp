#include "RHI.h"
#include "DebugLayer.h"
#include "Window.h"
#include "SwapChain.h"

FRHI GRHI;


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
		if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&GRHI.DxgiFactory))))
		{
			std::string message = "RHI: Failed To Create Factory";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}

	//Create Device
	{
		//ToDo: Explicit Adapter, for Diagnostics
		if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&GRHI.Device))))
		{
			std::string message = "RHI: Failed To Create Device";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}
	
	//Create Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;
		
		if (FAILED(GRHI.Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&GRHI.CmdQueue))))
		{
			std::string message = "RHI: Failed To Create Command Queue";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}

	GSwapChain.Initialize();

	//Create Command Allocator
	{
		if (FAILED(GRHI.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&GRHI.CmdAllocator))))
		{
			std::string message = "RHI: Failed To Create Command Allocator";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}
	
	//Create Fence
	{
		if (FAILED(GRHI.Device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence))))
		{
			std::string message = "RHI: Failed To Create Fence";
			LogError(message, ELogType::Fatal);
			return false;
		}

		FenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (!FenceEvent)
		{
			std::string message = "RHI: Failed To Create Fence Event";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}

	//Create Command List
	{
		if (FAILED(GRHI.Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&GRHI.CmdList))))
		{
			std::string message = "RHI: Failed To Create Command List";
			LogError(message, ELogType::Fatal);
			return false;
		}
	}

	return true;
}

void FRHI::Shutdown()
{
	GRHI.CmdList.Release();
	GRHI.CmdAllocator.Release();

	if (FenceEvent)
	{
		CloseHandle(FenceEvent);
	}

	Fence.Release();
	GRHI.CmdQueue.Release();
	GRHI.Device.Release();
	GRHI.DxgiFactory.Release();
}

void FRHI::Flush()
{
	for (size_t i = 0; i < GSwapChain.GetBufferingCount(); ++i) 
	{ 
		SignalAndWait(); 
	}
}

void FRHI::SignalAndWait()
{
	GRHI.CmdQueue->Signal(Fence, ++FenceValue);

	if (SUCCEEDED(Fence->SetEventOnCompletion(FenceValue, FenceEvent)))
	{
		if (WaitForSingleObject(FenceEvent, 20000) != WAIT_OBJECT_0)
		{
			std::exit(-1);
		}
	}
	else
	{
		std::exit(-1);
	}
}
void FRHI::InitializeCommandList()
{
	GRHI.CmdAllocator->Reset();
	GRHI.CmdList->Reset(GRHI.CmdAllocator, nullptr);
}

void FRHI::ExecuteCommandList()
{
	if (SUCCEEDED(GRHI.CmdList->Close()))
	{
		ID3D12CommandList* commandLists[] = { GRHI.CmdList };
		GRHI.CmdQueue->ExecuteCommandLists(1, commandLists);
		SignalAndWait();
	};
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

	GRHI.CmdList->ResourceBarrier(1, &barrier);
}
