#include "PCH.h"
#include "D3D12DepthStencil.h"
#include "Window.h"
#include "D3D12DescriptorHeapManager.h"
#include "DebugUtils.h"

// Constructs and initializes the depth stencil resource and view
D3D12DepthStencil::D3D12DepthStencil()
    : m_dsvHandle(GD3D12DescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV))
{
	CreateResource();
	CreateDepthStencilView();
}

// Creates the depth stencil resource on the GPU
void D3D12DepthStencil::CreateResource()
{
	// Describe the depth stencil view
	m_depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	// Set optimized clear value for depth and stencil
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Reversed-Z: clear depth to 0.0 (far) for maximum precision
	depthOptimizedClearValue.DepthStencil.Depth = 0.0f;
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
	depthStencilResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilResourceDesc.MipLevels = 1;
	depthStencilResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilResourceDesc.Height = static_cast<UINT>(GWindow.GetHeight());
	depthStencilResourceDesc.Width = static_cast<UINT>(GWindow.GetWidth());
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.SampleDesc.Count = 1;
	depthStencilResourceDesc.SampleDesc.Quality = 0;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	// Create the committed resource for depth stencil
	CHECK(GD3D12Rhi.GetDevice()->CreateCommittedResource(&heapDefaultProperties,
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
	GD3D12Rhi.GetDevice()->CreateDepthStencilView(m_resource.Get(), &m_depthStencilDesc, GetCPUHandle());
}

void D3D12DepthStencil::Clear() noexcept
{
	// Clear both depth and stencil. Reversed-Z convention clears depth to 0.0f.
	GD3D12Rhi.GetCommandList()->ClearDepthStencilView(
	    GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 0.0f, 0, 0, nullptr);
}

// Transition depth buffer to write state before rendering
void D3D12DepthStencil::SetWriteState() noexcept
{
	// Transition to depth-write for rendering.
	GD3D12Rhi.SetBarrier(m_resource.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

// Transition depth buffer to read state after rendering
void D3D12DepthStencil::SetReadState() noexcept
{
	// Transition to depth-read for shader sampling / post-processing.
	GD3D12Rhi.SetBarrier(m_resource.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
}

// Destructor resets resources
D3D12DepthStencil::~D3D12DepthStencil() noexcept
{
	// Release GPU resource and free descriptor handle. Descriptor manager handles no-op for invalid handles.
	m_resource.Reset();
	if (m_dsvHandle.IsValid())
	{
		GD3D12DescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_dsvHandle);
	}
}
