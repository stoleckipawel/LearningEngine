#pragma once

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
	// Initializes all descriptor heaps.
	void Initialize();
	
	void SetShaderVisibleHeapsScene();
	void SetShaderVisibleHeapsUI();

	// Releases all descriptor heap resources.
	void Release();

	// Returns reference to the CBV/SRV/UAV heap.
	DescriptorHeap& GetCBVSRVUAVHeap() { return *m_CBVSRVUAVHeap; }

	// Returns reference to the Sampler heap.
	DescriptorHeap& GetSamplerHeap() { return *m_SamplerHeap; }

	// Returns reference to the Depth Stencil View heap.
	DescriptorHeap& GetDepthStencilViewHeap() { return *m_DepthStencilViewHeap; }

	// Returns reference to the Render Target View heap.
	DescriptorHeap& GetRenderTargetViewHeap() { return *m_RenderTargetViewHeap; }
private:
	void SetShaderVisibleHeaps(ComPointer<ID3D12GraphicsCommandList7> cmdList);	
private:
	std::unique_ptr<DescriptorHeap> m_CBVSRVUAVHeap;        // CBV/SRV/UAV heap (shader visible)
	std::unique_ptr<DescriptorHeap> m_SamplerHeap;          // Sampler heap (shader visible)
	std::unique_ptr<DescriptorHeap> m_DepthStencilViewHeap; // Depth Stencil View heap
	std::unique_ptr<DescriptorHeap> m_RenderTargetViewHeap; // Render Target View heap
};

// Global instance for engine-wide access
extern DescriptorHeapManager GDescriptorHeapManager;