#include "PCH.h"
#include "D3D12Rhi.h"
#include "D3D12DebugLayer.h"
#include "Window.h"

// Constructs and initializes the RHI and all required resources
D3D12Rhi::D3D12Rhi(bool requireDXRSupport) noexcept
{
#if ENGINE_GPU_VALIDATION
	m_debugLayer = std::make_unique<D3D12DebugLayer>();
#endif
	CreateFactory();
	CreateDevice(requireDXRSupport);

#if ENGINE_GPU_VALIDATION
	m_debugLayer->InitializeInfoQueue(m_Device.Get());
#endif

	CheckShaderModel6Support();
	CreateCommandQueue();
	CreateCommandAllocators();
	CreateCommandLists();
	CreateFenceAndEvent();
}

// Selects the best available adapter (GPU) that supports Direct3D 12
void D3D12Rhi::SelectAdapter() noexcept
{
	const DXGI_GPU_PREFERENCE pref =
	    EngineSettings::PreferHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

	// Try adapter-by-preference first. Use a local temporary adapter to avoid
	// repeatedly replacing the member until a suitable one is found.
	for (UINT i = 0;; ++i)
	{
		ComPtr<IDXGIAdapter1> candidate;
		HRESULT hr = m_DxgiFactory->EnumAdapterByGpuPreference(i, pref, IID_PPV_ARGS(candidate.ReleaseAndGetAddressOf()));
		if (hr != S_OK)
			break;  // no more adapters or error

		DXGI_ADAPTER_DESC1 desc{};
		if (FAILED(candidate->GetDesc1(&desc)))
			continue;
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;  // skip WARP

		// Lightweight feature probe: does this adapter support D3D12 at the
		// desired feature level? We don't create a device here, just test.
		if (SUCCEEDED(D3D12CreateDevice(candidate.Get(), m_DesiredD3DFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
			m_Adapter = candidate;  // accept
			return;
		}
	}

	// Fallback enumerating adapters1 in classic order
	for (UINT i = 0;; ++i)
	{
		ComPtr<IDXGIAdapter1> candidate;
		HRESULT hr = m_DxgiFactory->EnumAdapters1(i, candidate.ReleaseAndGetAddressOf());
		if (hr != S_OK)
			break;

		DXGI_ADAPTER_DESC1 desc{};
		if (FAILED(candidate->GetDesc1(&desc)))
			continue;
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

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
		CHECK(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdAllocator[i].ReleaseAndGetAddressOf())));
	}
}

void D3D12Rhi::CreateCommandLists()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		CHECK(m_Device->CreateCommandList(
		    0,
		    D3D12_COMMAND_LIST_TYPE_DIRECT,
		    m_CmdAllocator[i].Get(),
		    nullptr,
		    IID_PPV_ARGS(m_CmdList[i].ReleaseAndGetAddressOf())));

		// Close immediately - command lists are created in recording state,
		// but we want them closed so BeginFrame can reset allocator then reopen.
		CHECK(m_CmdList[i]->Close());
	}
}

void D3D12Rhi::CreateFenceAndEvent()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_FenceValues[i] = 0;
	}

	CHECK(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf())));

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (!m_FenceEvent)
	{
		LOG_FATAL("Failed To Create Fence Event");
	}
}

void D3D12Rhi::CloseCommandList(uint32_t frameInFlightIndex) noexcept
{
	CHECK(m_CmdList[frameInFlightIndex]->Close());
}

void D3D12Rhi::ResetCommandAllocator(uint32_t frameInFlightIndex) noexcept
{
	if (!m_CmdAllocator[frameInFlightIndex])
	{
		LOG_FATAL("ResetCommandAllocator called with missing allocator");
		return;
	}

	CHECK(m_CmdAllocator[frameInFlightIndex]->Reset());
}

void D3D12Rhi::ResetCommandList(uint32_t frameInFlightIndex) noexcept
{
	if (!m_CmdList[frameInFlightIndex])
	{
		LOG_FATAL("ResetCommandList called without a valid command list");
		return;
	}
	if (!m_CmdAllocator[frameInFlightIndex])
	{
		LOG_FATAL("ResetCommandList called with missing allocator");
		return;
	}

	CHECK(m_CmdList[frameInFlightIndex]->Reset(m_CmdAllocator[frameInFlightIndex].Get(), nullptr));
}

void D3D12Rhi::ExecuteCommandList(uint32_t frameInFlightIndex) noexcept
{
	if (!m_CmdList[frameInFlightIndex] || !m_CmdQueue)
	{
		LOG_FATAL("ExecuteCommandList called without valid command list or queue");
	}

	ID3D12CommandList* ppcommandLists[] = {m_CmdList[frameInFlightIndex].Get()};
	m_CmdQueue->ExecuteCommandLists(1, ppcommandLists);
}

void D3D12Rhi::SetBarrier(
    uint32_t frameInFlightIndex,
    ID3D12Resource* Resource,
    D3D12_RESOURCE_STATES StateBefore,
    D3D12_RESOURCE_STATES StateAfter) noexcept
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = Resource;
	// Transition all subresources (depth + stencil planes)
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = StateBefore;
	barrier.Transition.StateAfter = StateAfter;

	if (!m_CmdList[frameInFlightIndex])
	{
		LOG_FATAL("SetBarrier: command list is null");
	}

	m_CmdList[frameInFlightIndex]->ResourceBarrier(1, &barrier);
}

void D3D12Rhi::WaitForGPU(uint32_t frameInFlightIndex) noexcept
{
	// TODO: Implement WaitForMultipleObjects for correct frame buffering & pacing

	const uint64_t fenceCurrentValue = m_FenceValues[frameInFlightIndex];
	if (!m_Fence)
	{
		LOG_FATAL("WaitForGPU called without a fence");
	}

	const uint64_t fenceCompletedValue = m_Fence->GetCompletedValue();
	if (fenceCompletedValue < fenceCurrentValue)
	{
		CHECK(m_Fence->SetEventOnCompletion(fenceCurrentValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void D3D12Rhi::Signal(uint32_t frameInFlightIndex) noexcept
{
	// Schedule a Signal command in the queue. -> Updates Fence Completed Value
	const uint64_t currentFenceValue = m_NextFenceValue++;
	if (!m_CmdQueue || !m_Fence)
	{
		LOG_FATAL("Signal called without command queue or fence");
	}

	CHECK(m_CmdQueue->Signal(m_Fence.Get(), currentFenceValue));

	// Set the fence value for the next frame.
	m_FenceValues[frameInFlightIndex] = currentFenceValue;
}

void D3D12Rhi::Flush() noexcept
{
	// Signal and wait for all frames to complete
	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		Signal(i);
		WaitForGPU(i);
	}
}

D3D12Rhi::~D3D12Rhi() noexcept
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_CmdList[i].Reset();
		m_CmdAllocator[i].Reset();
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
	if (m_debugLayer)
	{
		m_debugLayer->ReportLiveDeviceObjects(m_Device.Get());
	}
#endif

	m_Device.Reset();
	m_Adapter.Reset();
	m_DxgiFactory.Reset();

#if ENGINE_GPU_VALIDATION
	m_debugLayer.reset();  // Destroy after device to report live objects
#endif
}