#include "PCH.h"
#include "D3D12Rhi.h"
#include "D3D12DebugLayer.h"
#include "Window.h"

D3D12Rhi GD3D12Rhi;

// Selects the best available adapter (GPU) that supports Direct3D 12
void D3D12Rhi::SelectAdapter() noexcept
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
void D3D12Rhi::CheckShaderModel6Support() const noexcept
{
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
	shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
	if (!m_Device)
	{
		LOG_FATAL("CheckShaderModel6Support called before device creation");
	}

	HRESULT hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	if (FAILED(hr) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)
	{
		LOG_FATAL("Device does not support Shader Model 6.0. Minimum required for engine.");
	}
}

// Initializes the RHI and all required resources
void D3D12Rhi::Initialize(bool RequireDXRSupport) noexcept
{
#if ENGINE_GPU_VALIDATION
	GD3D12DebugLayer.Initialize();
#endif
	CreateFactory();
	CreateDevice(RequireDXRSupport);

#if ENGINE_GPU_VALIDATION
		GD3D12DebugLayer.InitializeInfoQueue();
#endif

	CheckShaderModel6Support();
	CreateCommandQueue();
	CreateCommandAllocators();
	CreateCommandLists();
	CreateFenceAndEvent();
}

void D3D12Rhi::CreateFactory()
{
#if ENGINE_GPU_VALIDATION
	UINT dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT dxgiFactoryFlags = 0;
#endif
	CHECK(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_DxgiFactory.ReleaseAndGetAddressOf())));
}

void D3D12Rhi::CreateDevice(bool /*requireDXRSupport*/)
{
	SelectAdapter();
	if (!m_Adapter)
	{
		LOG_FATAL("No suitable adapter found when creating device");
	}

	CHECK(D3D12CreateDevice(m_Adapter.Get(), m_DesiredD3DFeatureLevel, IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf())));
}

void D3D12Rhi::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	CHECK(m_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_CmdQueue.ReleaseAndGetAddressOf())));
}

void D3D12Rhi::CreateCommandAllocators()
{
	for (size_t i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		CHECK(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdAllocatorScene[i].ReleaseAndGetAddressOf())));
	}
}

void D3D12Rhi::CreateCommandLists()
{
	if (!m_Device)
	{
		LOG_FATAL("CreateCommandLists called before device creation");
		return;
	}

	if (!m_CmdAllocatorScene[GD3D12SwapChain.GetFrameInFlightIndex()])
	{
		LOG_FATAL("CreateCommandLists: command allocator missing for current frame");
		return;
	}

		CHECK(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocatorScene[GD3D12SwapChain.GetFrameInFlightIndex()].Get(), nullptr, IID_PPV_ARGS(m_CmdListScene.ReleaseAndGetAddressOf())));
}

void D3D12Rhi::CreateFenceAndEvent()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_FenceValues[i] = 0;
	}

	if (!m_Device)
	{
		LOG_FATAL("CreateFenceAndEvent called before device creation");
		return;
	}

	CHECK(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf())));

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_FenceEvent)
	{
		LOG_FATAL("Failed To Create Fence Event");
	}
}

void D3D12Rhi::CloseCommandListScene() noexcept
{
	CHECK(m_CmdListScene->Close());
}

void D3D12Rhi::ResetCommandAllocator() noexcept
{
	if (!m_CmdAllocatorScene[GD3D12SwapChain.GetFrameInFlightIndex()])
	{
		LOG_FATAL("ResetCommandAllocator called with missing allocator");
		return;
	}

	CHECK(m_CmdAllocatorScene[GD3D12SwapChain.GetFrameInFlightIndex()]->Reset());
}

void D3D12Rhi::ResetCommandList() noexcept
{
	if (!m_CmdListScene)
	{
		LOG_FATAL("ResetCommandList called without a valid command list");
		return;
	}
	if (!m_CmdAllocatorScene[GD3D12SwapChain.GetFrameInFlightIndex()])
	{
		LOG_FATAL("ResetCommandList called with missing allocator");
		return;
	}

	CHECK(m_CmdListScene->Reset(m_CmdAllocatorScene[GD3D12SwapChain.GetFrameInFlightIndex()].Get(), nullptr));
}

// Executes the current command list on the command queue
void D3D12Rhi::ExecuteCommandList() noexcept
{
	if (!m_CmdListScene || !m_CmdQueue)
	{
		LOG_FATAL("ExecuteCommandList called without valid command list or queue");
	}

	ID3D12CommandList* ppcommandLists[] = { m_CmdListScene.Get() };
	m_CmdQueue->ExecuteCommandLists(1, ppcommandLists);
}


// Sets a resource barrier for a resource state transition
void D3D12Rhi::SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) noexcept
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
		LOG_FATAL("SetBarrier: command list is null");
	}

	m_CmdListScene->ResourceBarrier(1, &barrier);
}

// Waits for the GPU to finish executing commands
void D3D12Rhi::WaitForGPU() noexcept
{
	//ToDO: Implement Wait For Multiple Objects
	//Essential for correct frame bufering & frame pacing!!!

	const UINT64 fenceCurrentValue = m_FenceValues[GD3D12SwapChain.GetFrameInFlightIndex()];
	if (!m_Fence)
	{
		LOG_FATAL("WaitForGPU called without a fence");
	}

	const UINT64 fenceCompletedValue = m_Fence->GetCompletedValue();
	if (fenceCompletedValue < fenceCurrentValue)
	{
		CHECK(m_Fence->SetEventOnCompletion(fenceCurrentValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

// Signals the fence for synchronization
void D3D12Rhi::Signal() noexcept
{
	// Schedule a Signal command in the queue. -> Updates Fence Completed Value
	const UINT64 currentFenceValue = m_NextFenceValue++;
	if (!m_CmdQueue || !m_Fence)
	{
		LOG_FATAL("Signal called without command queue or fence");
	}

	CHECK(m_CmdQueue->Signal(m_Fence.Get(), currentFenceValue));

	// Set the fence value for the next frame.
	m_FenceValues[GD3D12SwapChain.GetFrameInFlightIndex()] = currentFenceValue;
}

// Flushes the command queue (signal and wait)
void D3D12Rhi::Flush() noexcept
{
	Signal();
	WaitForGPU();
}

// Releases all resources and shuts down the RHI
void D3D12Rhi::Shutdown() noexcept
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
		GD3D12DebugLayer.ReportLiveDeviceObjects();
	#endif
	
	m_Device.Reset();
	m_Adapter.Reset();
	m_DxgiFactory.Reset();

	#if ENGINE_GPU_VALIDATION
		GD3D12DebugLayer.Shutdown();

	#endif
}