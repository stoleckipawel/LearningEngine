#include "PCH.h"
#include "D3D12/RHI.h"
#include "D3D12/DebugLayer.h"
#include "Window.h"

RHI GRHI;

// Selects the best available adapter (GPU) that supports Direct3D 12
void RHI::SelectAdapter()
{
	bool bHighPerformancePreference = true;
	enum DXGI_GPU_PREFERENCE GpuPreference = bHighPerformancePreference ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

	// Try to find an adapter that meets the required GPU preference.
	for(UINT AdapterIndex = 0; SUCCEEDED(m_DxgiFactory->EnumAdapterByGpuPreference(AdapterIndex, GpuPreference, IID_PPV_ARGS(m_Adapter.ReleaseAndGetAddressOf())));++AdapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		m_Adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Reject software adapters.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	// If the above failed, fall back to the default adapter.
	if(m_Adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(m_DxgiFactory->EnumAdapters1(adapterIndex, m_Adapter.ReleaseAndGetAddressOf())); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			m_Adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Reject software adapters.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}   
}

// Checks for Shader Model 6.0 support
void RHI::CheckShaderModel6Support()
{
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
	if (m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)
	{
		LogMessage("RHI: Device does not support Shader Model 6.0. Minimum required for engine.", ELogType::Fatal);
	}
}

// Initializes the RHI and all required resources
void RHI::Initialize(bool RequireDXRSupport)
{
	GDebugLayer.Initialize(); 
	CreateFactory();
	CreateDevice(RequireDXRSupport);
	GDebugLayer.InitializeInfoQueue();
	CheckShaderModel6Support();
	CreateCommandQueue();
	CreateCommandAllocators();
	CreateCommandLists();
	CreateFenceAndEvent();
}

void RHI::CreateFactory()
{
#if defined(_DEBUG)
	UINT dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT dxgiFactoryFlags = 0;
#endif
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_DxgiFactory.ReleaseAndGetAddressOf())), "RHI: Failed To Create Factory");
}

void RHI::CreateDevice(bool /*requireDXRSupport*/)
{
	SelectAdapter();
	ThrowIfFailed(D3D12CreateDevice(m_Adapter.Get(), m_DesiredD3DFeatureLevel, IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf())), "RHI: Failed To Create Device");
}

void RHI::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_CmdQueue.ReleaseAndGetAddressOf())), "RHI: Failed To Create Command Queue");
}

void RHI::CreateCommandAllocators()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdAllocatorScene[i].ReleaseAndGetAddressOf())), "RHI: Failed To Create Scene Command Allocator");
	}
}

void RHI::CreateCommandLists()
{
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()].Get(), nullptr, IID_PPV_ARGS(m_CmdListScene.ReleaseAndGetAddressOf())), "RHI: Failed To Create Scene Command List");
}

void RHI::CreateFenceAndEvent()
{
	for (UINT i = 0; i < NumFramesInFlight; ++i)
	{
		m_FenceValues[i] = 0;
	}

	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf())), "RHI: Failed To Create Fence");

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_FenceEvent)
	{
		LogMessage("RHI: Failed To Create Fence Event", ELogType::Fatal);
	}
}

void RHI::CloseCommandListScene()
{
	ThrowIfFailed(m_CmdListScene->Close(), "RHI: Failed To Close Scene Command List");
}

void RHI::ResetCommandAllocator()
{
	ThrowIfFailed(m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()]->Reset(), "RHI: Failed To Reset Scene Command Allocator");
}

void RHI::ResetCommandList()
{
	ThrowIfFailed(m_CmdListScene->Reset(m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()].Get(), nullptr), "RHI: Failed To Reset Scene Command List");
}

// Executes the current command list on the command queue
void RHI::ExecuteCommandList()
{
	ID3D12CommandList* ppcommandLists[] = { m_CmdListScene.Get() };
	m_CmdQueue->ExecuteCommandLists(1, ppcommandLists);
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

	m_CmdListScene->ResourceBarrier(1, &barrier);
}

// Waits for the GPU to finish executing commands
void RHI::WaitForGPU()
{
	//ToDO: Implement Wait For Multiple Objects
	//Essential for correct frame bufering & frame pacing!!!

	UINT FenceCurrentValue = m_FenceValues[GSwapChain.GetFrameInFlightIndex()];
	UINT FenceCompletedValue = m_Fence->GetCompletedValue();

	if (FenceCompletedValue < FenceCurrentValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(FenceCurrentValue, m_FenceEvent), "RHI: Failed To Signal Command Queue");
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

// Signals the fence for synchronization
void RHI::Signal()
{
	// Schedule a Signal command in the queue. -> Updates Fence Completed Value
	UINT64 currentFenceValue = m_NextFenceValue++;
	ThrowIfFailed(m_CmdQueue->Signal(m_Fence.Get(), currentFenceValue), "RHI: Failed To Signal Command Queue");

	// Set the fence value for the next frame.
	m_FenceValues[GSwapChain.GetFrameInFlightIndex()] = currentFenceValue;
}

// Flushes the command queue (signal and wait)
void RHI::Flush()
{
	Signal();
	WaitForGPU();
}

// Releases all resources and shuts down the RHI
void RHI::Shutdown()
{
	m_CmdListScene.Reset();

	for (UINT i = 0; i < NumFramesInFlight; ++i)
	{
		m_CmdAllocatorScene[i].Reset();
		m_FenceValues[i] = 0;
	}

	if (m_FenceEvent)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}

	m_Fence.Reset();
	m_CmdQueue.Reset();
	GDebugLayer.ReportLiveDeviceObjects();
	m_Device.Reset();
	m_Adapter.Reset();
	m_DxgiFactory.Reset();
	GDebugLayer.Shutdown();
}