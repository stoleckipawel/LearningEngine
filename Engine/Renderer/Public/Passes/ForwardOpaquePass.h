// ============================================================================
// ForwardOpaquePass.h
// ----------------------------------------------------------------------------
// Forward-rendering pass for opaque geometry. Binds pipeline state, constant
// buffers, textures, samplers, and draws all opaque mesh draw commands from
// the current SceneView.
//
// USAGE:
//   frameGraph.AddPass<ForwardOpaquePass>("ForwardOpaque",
//       rootSig, pso, cbManager, heapManager, texManager, samplerLib,
//       meshCache, swapChain, depthStencil);
//
// DESIGN:
//   - Derives from RenderPass for FrameGraph integration
//   - Constructor-injected dependencies (non-owning references)
//   - Setup captures SceneView pointer and declares resource usage
//   - Execute records all draw commands through RenderContext
//   - MVP: directly calls swap chain / depth stencil for transitions
//
// NOTES:
//   - Created and owned by FrameGraph via AddPass<T>()
//   - Dependencies must outlive this pass
//   - When FrameGraph matures, resource transitions will move out
// ============================================================================

#pragma once

#include "Renderer/Public/FrameGraph/RenderPass.h"
#include "Renderer/Public/FrameGraph/ResourceHandle.h"

class D3D12ConstantBufferManager;
class D3D12DepthStencil;
class D3D12DescriptorHeapManager;
class D3D12PipelineState;
class D3D12RootSignature;
class D3D12SamplerLibrary;
class D3D12SwapChain;
class GPUMeshCache;
class TextureManager;

// ============================================================================
// ForwardOpaquePass
// ============================================================================

class ForwardOpaquePass final : public RenderPass
{
  public:
	ForwardOpaquePass(
	    std::string_view name,
	    D3D12RootSignature& rootSignature,
	    D3D12PipelineState& pipelineState,
	    D3D12ConstantBufferManager& constantBufferManager,
	    D3D12DescriptorHeapManager& descriptorHeapManager,
	    TextureManager& textureManager,
	    D3D12SamplerLibrary& samplerLibrary,
	    GPUMeshCache& gpuMeshCache,
	    D3D12SwapChain& swapChain,
	    D3D12DepthStencil& depthStencil) noexcept;

	~ForwardOpaquePass() noexcept override = default;

	void Setup(PassBuilder& builder, const SceneView& sceneView) override;
	void Execute(RenderContext& context) override;

  private:
	// -------------------------------------------------------------------------
	// Dependencies (not owned)
	// -------------------------------------------------------------------------

	D3D12RootSignature* m_rootSignature = nullptr;
	D3D12PipelineState* m_pipelineState = nullptr;
	D3D12ConstantBufferManager* m_constantBufferManager = nullptr;
	D3D12DescriptorHeapManager* m_descriptorHeapManager = nullptr;
	TextureManager* m_textureManager = nullptr;
	D3D12SamplerLibrary* m_samplerLibrary = nullptr;
	GPUMeshCache* m_gpuMeshCache = nullptr;
	D3D12SwapChain* m_swapChain = nullptr;
	D3D12DepthStencil* m_depthStencil = nullptr;

	// -------------------------------------------------------------------------
	// Per-frame state (set during Setup, valid until next Setup call)
	// -------------------------------------------------------------------------

	const SceneView* m_sceneView = nullptr;
	ResourceHandle m_backBuffer;
	ResourceHandle m_depthBuffer;
};
