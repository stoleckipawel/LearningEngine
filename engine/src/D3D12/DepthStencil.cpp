#include "Core/PCH.h"
#include "D3D12/DepthStencil.h"
#include "Core/Window.h"
#include "D3D12/DescriptorHeapManager.h"

// Constructs and initializes the depth stencil resource and view
DepthStencil::DepthStencil(UINT descriptorHandleIndex)
	: m_DescriptorHandleIndex(descriptorHandleIndex)
{
	CreateResource();
	CreateView();
}

// Creates the depth stencil resource on the GPU
void DepthStencil::CreateResource()
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
	depthStencilResourceDesc.Height = GWindow.GetHeight();
	depthStencilResourceDesc.Width = GWindow.GetWidth();
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.SampleDesc.Count = 1;
	depthStencilResourceDesc.SampleDesc.Quality = 0;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	// Create the committed resource for depth stencil
	ThrowIfFailed(
		GRHI.Device->CreateCommittedResource(
			&heapDefaultProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_READ,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&m_resource)),
		"Depth Stencil: Failed To Create Resource"
	);
}

// Creates the depth stencil view in the descriptor heap
void DepthStencil::CreateView()
{
	GRHI.Device->CreateDepthStencilView(m_resource, &m_depthStencilDesc, GetCPUHandle());
}

// Returns the GPU descriptor handle for shader access
D3D12_GPU_DESCRIPTOR_HANDLE DepthStencil::GetGPUHandle()
{
	return GDescriptorHeapManager.GetDepthStencilViewHeap().GetGPUHandle(m_DescriptorHandleIndex);
}

// Returns the CPU descriptor handle for descriptor heap management
D3D12_CPU_DESCRIPTOR_HANDLE DepthStencil::GetCPUHandle()
{
	return GDescriptorHeapManager.GetDepthStencilViewHeap().GetCPUHandle(m_DescriptorHandleIndex);
} 

// Clears the depth stencil view
void DepthStencil::Clear()
{
	// Reversed-Z clear values: depth=0.0, stencil=0
	GRHI.GetCommandList()->ClearDepthStencilView(GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 0.0f, 0, 0, nullptr);
}

// Transition depth buffer to write state before rendering
void DepthStencil::SetWriteState()
{
	GRHI.SetBarrier(
		m_resource,
		D3D12_RESOURCE_STATE_DEPTH_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);
}

// Transition depth buffer to read state after rendering
void DepthStencil::SetReadState()
{
	GRHI.SetBarrier(
		m_resource,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_DEPTH_READ
	);
}

// Releases the depth stencil resource
void DepthStencil::Release()
{
	m_resource.Release();
}

// Destructor releases resources
DepthStencil::~DepthStencil()
{
	Release();
}
