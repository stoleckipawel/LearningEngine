#include "PCH.h"
#include "D3D12DepthStencil.h"
#include "D3D12Rhi.h"
#include "Window.h"
#include "D3D12DescriptorHeapManager.h"
#include "DebugUtils.h"
#include "DepthConvention.h"
#include "EngineConfig.h"

// Constructs and initializes the depth stencil resource and view
D3D12DepthStencil::D3D12DepthStencil(D3D12Rhi& rhi, Window& window, D3D12DescriptorHeapManager& descriptorHeapManager) :
    m_rhi(rhi),
    m_dsvHandle(descriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)),
    m_window(&window),
    m_descriptorHeapManager(&descriptorHeapManager)
{
	CreateResource();
	CreateDepthStencilView();
}

// Creates the depth stencil resource on the GPU
void D3D12DepthStencil::CreateResource()
{
	// Describe the depth stencil view
	m_depthStencilDesc.Format = EngineSettings::DepthStencilFormat;
	m_depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	// Set optimized clear value for depth and stencil
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = EngineSettings::DepthStencilFormat;
	depthOptimizedClearValue.DepthStencil.Depth = DepthConvention::GetClearDepth();
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	// Heap properties for default heap
	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

	// Describe the depth stencil resource
	D3D12_RESOURCE_DESC depthStencilResourceDesc = {};
	depthStencilResourceDesc.Format = EngineSettings::DepthStencilFormat;
	depthStencilResourceDesc.MipLevels = 1;
	depthStencilResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilResourceDesc.Height = static_cast<UINT>(m_window->GetHeight());
	depthStencilResourceDesc.Width = static_cast<UINT>(m_window->GetWidth());
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.SampleDesc.Count = 1;
	depthStencilResourceDesc.SampleDesc.Quality = 0;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	// Create the committed resource for depth stencil
	CHECK(m_rhi.GetDevice()->CreateCommittedResource(
	    &heapDefaultProperties,
	    D3D12_HEAP_FLAG_NONE,
	    &depthStencilResourceDesc,
	    D3D12_RESOURCE_STATE_DEPTH_READ,
	    &depthOptimizedClearValue,
	    IID_PPV_ARGS(m_resource.ReleaseAndGetAddressOf())));

	// Name the resource for easier debugging (no-op in release via DebugUtils)
	DebugUtils::SetDebugName(m_resource, L"RHI_DepthStencil");
}

// Creates the depth stencil view in the descriptor heap
void D3D12DepthStencil::CreateDepthStencilView()
{
	// Create the depth-stencil view in the allocated descriptor slot
	m_rhi.GetDevice()->CreateDepthStencilView(m_resource.Get(), &m_depthStencilDesc, GetCPUHandle());
}

void D3D12DepthStencil::Clear() noexcept
{
	// Clear depth to convention-appropriate value (0.0 for reversed-Z, 1.0 for standard)
	const float clearDepth = DepthConvention::GetClearDepth();
	m_rhi.GetCommandList()
	    ->ClearDepthStencilView(GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearDepth, 0, 0, nullptr);
}

// Transition depth buffer to write state before rendering
void D3D12DepthStencil::SetWriteState() noexcept
{
	// Transition to depth-write for rendering.
	m_rhi.SetBarrier(m_resource.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

// Transition depth buffer to read state after rendering
void D3D12DepthStencil::SetReadState() noexcept
{
	// Transition to depth-read for shader sampling / post-processing.
	m_rhi.SetBarrier(m_resource.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
}

D3D12DepthStencil::~D3D12DepthStencil() noexcept
{
	// Release GPU resource and free descriptor handle. Descriptor manager handles no-op for invalid handles.
	m_resource.Reset();
	if (m_dsvHandle.IsValid())
	{
		m_descriptorHeapManager->FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_dsvHandle);
	}
}
