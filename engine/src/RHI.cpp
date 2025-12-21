#include "PCH.h"
#include "RHI.h"
#include "DebugLayer.h"
#include "Window.h"

RHI GRHI;

// Selects the best available adapter (GPU) that supports Direct3D 12
void RHI::SelectAdapter() noexcept
{
	const DXGI_GPU_PREFERENCE pref = EngineSettings::PreferHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

	// Try adapter-by-preference first. Use a local temporary adapter to avoid
	// repeatedly replacing the member until a suitable one is found.
	for (UINT i = 0; ; ++i)
	{
		ComPtr<IDXGIAdapter1> candidate;
		HRESULT hr = m_DxgiFactory->EnumAdapterByGpuPreference(i, pref, IID_PPV_ARGS(candidate.ReleaseAndGetAddressOf()));
		if (hr != S_OK) break; // no more adapters or error

		DXGI_ADAPTER_DESC1 desc{};
		if (FAILED(candidate->GetDesc1(&desc))) continue;
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue; // skip WARP

		// Lightweight feature probe: does this adapter support D3D12 at the
		// desired feature level? We don't create a device here, just test.
		if (SUCCEEDED(D3D12CreateDevice(candidate.Get(), m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
			m_Adapter = candidate; // accept
			return;
		}
	}

	// Fallback enumerating adapters1 in classic order
	for (UINT i = 0; ; ++i)
	{
		ComPtr<IDXGIAdapter1> candidate;
		HRESULT hr = m_DxgiFactory->EnumAdapters1(i, candidate.ReleaseAndGetAddressOf());
		if (hr != S_OK) break;

		DXGI_ADAPTER_DESC1 desc{};
		if (FAILED(candidate->GetDesc1(&desc))) continue;
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		if (SUCCEEDED(D3D12CreateDevice(candidate.Get(), m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
			m_Adapter = candidate;
			return;
		}
	}

	// If not found, leave m_Adapter null; caller will handle this failure.
}

// Checks for Shader Model 6.0 support
void RHI::CheckShaderModel6Support() const noexcept
{
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
	shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
	if (!m_Device)
	{
		LogMessage("CheckShaderModel6Support called before device creation", ELogType::Fatal);
		return;
	}

	HRESULT hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	if (FAILED(hr) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)
	{
		LogMessage("Device does not support Shader Model 6.0. Minimum required for engine.", ELogType::Fatal);
	}
}

// Initializes the RHI and all required resources
void RHI::Initialize(bool RequireDXRSupport) noexcept
{
#if ENGINE_GPU_VALIDATION
	GDebugLayer.Initialize();
#endif
	CreateFactory();
	CreateDevice(RequireDXRSupport);

#if ENGINE_GPU_VALIDATION
	GDebugLayer.InitializeInfoQueue();
#endif

	CheckShaderModel6Support();
	CreateCommandQueue();
	CreateCommandAllocators();
	CreateCommandLists();
	CreateFenceAndEvent();
}

void RHI::CreateFactory()
{
#if ENGINE_GPU_VALIDATION
	UINT dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT dxgiFactoryFlags = 0;
#endif
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_DxgiFactory.ReleaseAndGetAddressOf())), "Failed To Create Factory");
}

void RHI::CreateDevice(bool /*requireDXRSupport*/)
{
	SelectAdapter();
	if (!m_Adapter)
	{
		LogMessage("No suitable adapter found when creating device", ELogType::Fatal);
	}

	ThrowIfFailed(D3D12CreateDevice(m_Adapter.Get(), m_DesiredD3DFeatureLevel, IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf())), "Failed To Create Device");
}

void RHI::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_CmdQueue.ReleaseAndGetAddressOf())), "Failed To Create Command Queue");
}

void RHI::CreateCommandAllocators()
{
	for (size_t i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdAllocatorScene[i].ReleaseAndGetAddressOf())), "Failed To Create Scene Command Allocator");
	}
}

void RHI::CreateCommandLists()
{
	if (!m_Device)
	{
		LogMessage("CreateCommandLists called before device creation", ELogType::Fatal);
		return;
	}

	if (!m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()])
	{
		LogMessage("CreateCommandLists: command allocator missing for current frame", ELogType::Fatal);
		return;
	}

	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()].Get(), nullptr, IID_PPV_ARGS(m_CmdListScene.ReleaseAndGetAddressOf())), "Failed To Create Scene Command List");
}

void RHI::CreateFenceAndEvent()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_FenceValues[i] = 0;
	}

	if (!m_Device)
	{
		LogMessage("CreateFenceAndEvent called before device creation", ELogType::Fatal);
		return;
	}

	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf())), "Failed To Create Fence");

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_FenceEvent)
	{
		LogMessage("Failed To Create Fence Event", ELogType::Fatal);
	}
}

void RHI::CloseCommandListScene() noexcept
{
	ThrowIfFailed(m_CmdListScene->Close(), "Failed To Close Scene Command List");
}

void RHI::ResetCommandAllocator() noexcept
{
	if (!m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()])
	{
		LogMessage("ResetCommandAllocator called with missing allocator", ELogType::Fatal);
		return;
	}

	ThrowIfFailed(m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()]->Reset(), "Failed To Reset Scene Command Allocator");
}

void RHI::ResetCommandList() noexcept
{
	if (!m_CmdListScene)
	{
		LogMessage("ResetCommandList called without a valid command list", ELogType::Fatal);
		return;
	}
	if (!m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()])
	{
		LogMessage("ResetCommandList called with missing allocator", ELogType::Fatal);
		return;
	}

	ThrowIfFailed(m_CmdListScene->Reset(m_CmdAllocatorScene[GSwapChain.GetFrameInFlightIndex()].Get(), nullptr), "Failed To Reset Scene Command List");
}

// Executes the current command list on the command queue
void RHI::ExecuteCommandList() noexcept
{
	if (!m_CmdListScene || !m_CmdQueue)
	{
		LogMessage("ExecuteCommandList called without valid command list or queue", ELogType::Fatal);
	}

	ID3D12CommandList* ppcommandLists[] = { m_CmdListScene.Get() };
	m_CmdQueue->ExecuteCommandLists(1, ppcommandLists);
}


// Sets a resource barrier for a resource state transition
void RHI::SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) noexcept
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = Resource;
	// Transition all subresources (depth + stencil planes)
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = StateBefore;
	barrier.Transition.StateAfter = StateAfter;

	if (!m_CmdListScene)
	{
		LogMessage("SetBarrier: command list is null", ELogType::Fatal);
	}

	m_CmdListScene->ResourceBarrier(1, &barrier);
}

// Waits for the GPU to finish executing commands
void RHI::WaitForGPU() noexcept
{
	//ToDO: Implement Wait For Multiple Objects
	//Essential for correct frame bufering & frame pacing!!!

	const UINT64 fenceCurrentValue = m_FenceValues[GSwapChain.GetFrameInFlightIndex()];
	if (!m_Fence)
	{
		LogMessage("WaitForGPU called without a fence", ELogType::Fatal);
	}

	const UINT64 fenceCompletedValue = m_Fence->GetCompletedValue();
	if (fenceCompletedValue < fenceCurrentValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceCurrentValue, m_FenceEvent), "Failed To Signal Command Queue");
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

// Signals the fence for synchronization
void RHI::Signal() noexcept
{
	// Schedule a Signal command in the queue. -> Updates Fence Completed Value
	const UINT64 currentFenceValue = m_NextFenceValue++;
	if (!m_CmdQueue || !m_Fence)
	{
		LogMessage("Signal called without command queue or fence", ELogType::Fatal);
	}

	ThrowIfFailed(m_CmdQueue->Signal(m_Fence.Get(), currentFenceValue), "Failed To Signal Command Queue");

	// Set the fence value for the next frame.
	m_FenceValues[GSwapChain.GetFrameInFlightIndex()] = currentFenceValue;
}

// Flushes the command queue (signal and wait)
void RHI::Flush() noexcept
{
	Signal();
	WaitForGPU();
}

// Releases all resources and shuts down the RHI
void RHI::Shutdown() noexcept
{
	m_CmdListScene.Reset();

	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
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

	#if ENGINE_REPORT_LIVE_OBJECTS
		GDebugLayer.ReportLiveDeviceObjects();
	#endif
	
	m_Device.Reset();
	m_Adapter.Reset();
	m_DxgiFactory.Reset();

	#if ENGINE_GPU_VALIDATION
		GDebugLayer.Shutdown();
	#endif
}