#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "DescriptorHeap.h"

/**
 * Manages all descriptor heaps required by the engine.
 *
 * This class encapsulates creation, binding, and release of descriptor heaps for:
 * - CBV/SRV/UAV (shader visible)
 * - Sampler (shader visible)
 * - Depth Stencil View (DSV)
 * - Render Target View (RTV)
 *
 * Provides accessors for each heap and methods for initialization, binding, and cleanup.
 */
class DescriptorHeapManager
{
public:
	//Initializes all descriptor heaps.
	void Initialize();

	//Sets the shader-visible heaps for the current command list.
	void SetShaderVisibleHeaps();

	// Releases all descriptor heap resources.
	void Release();

	// Returns reference to the CBV/SRV/UAV heap.
	DescriptorHeap& GetCBVSRVUAVHeap() { return m_CBVSRVUAVHeap; }

	// Returns reference to the Sampler heap.
	DescriptorHeap& GetSamplerHeap() { return m_SamplerHeap; }

	// Returns reference to the Depth Stencil View heap.
	DescriptorHeap& GetDepthStencilViewHeap() { return m_DepthStencilViewHeap; }

	// Returns reference to the Render Target View heap.
	DescriptorHeap& GetRenderTargetViewHeap() { return m_RenderTargetViewHeap; }

private:
	DescriptorHeap m_CBVSRVUAVHeap;        // CBV/SRV/UAV heap (shader visible)
	DescriptorHeap m_SamplerHeap;          // Sampler heap (shader visible)
	DescriptorHeap m_DepthStencilViewHeap; // Depth Stencil View heap
	DescriptorHeap m_RenderTargetViewHeap; // Render Target View heap
};

// Global instance for engine-wide access
extern DescriptorHeapManager GDescriptorHeapManager;