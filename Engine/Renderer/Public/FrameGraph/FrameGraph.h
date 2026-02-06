#pragma once

#include "Renderer/Public/RendererAPI.h"
#include "Renderer/Public/FrameGraph/RenderPass.h"
#include "Renderer/Public/FrameGraph/PassBuilder.h"

#include <memory>
#include <vector>
#include <utility>

// Forward declarations
class D3D12SwapChain;
class D3D12DepthStencil;
class RenderContext;
struct SceneView;

/**
 * @brief Manages render passes and their execution order.
 *
 * The FrameGraph is responsible for:
 * - Registering render passes
 * - Calling Setup() on all passes to declare resource usage
 * - Compiling the graph (MVP: no-op, future: automatic barriers/ordering)
 * - Executing all passes in order
 *
 * Per-Frame Flow:
 *   1. SceneView view = scene.ExtractSceneView(w, h)
 *   2. frameGraph.Setup(view)      // All passes declare resources
 *   3. frameGraph.Compile()        // MVP: no-op
 *   4. frameGraph.Execute(context) // All passes record commands
 */
class SPARKLE_RENDERER_API FrameGraph
{
  public:
	/**
	 * @brief Construct a FrameGraph with access to render targets.
	 * @param swapChain Pointer to the swap chain for back buffer access.
	 * @param depthStencil Pointer to the depth stencil buffer.
	 */
	FrameGraph(D3D12SwapChain* swapChain, D3D12DepthStencil* depthStencil);
	~FrameGraph();

	// Non-copyable, movable
	FrameGraph(const FrameGraph&) = delete;
	FrameGraph& operator=(const FrameGraph&) = delete;
	FrameGraph(FrameGraph&&) noexcept = default;
	FrameGraph& operator=(FrameGraph&&) noexcept = default;

	/**
	 * @brief Create and register a render pass.
	 * @tparam T The concrete RenderPass type to create.
	 * @tparam Args Constructor argument types.
	 * @param name The name of the pass (for debugging).
	 * @param args Additional arguments forwarded to the pass constructor.
	 * @return Reference to the created pass.
	 */
	template <typename T, typename... Args> T& AddPass(std::string_view name, Args&&... args)
	{
		static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
		auto pass = std::make_unique<T>(name, std::forward<Args>(args)...);
		T& ref = *pass;
		m_passes.push_back(std::move(pass));
		return ref;
	}

	/**
	 * @brief Setup all passes for the current frame.
	 *
	 * Calls Setup() on each registered pass, allowing them to declare
	 * their resource requirements via the PassBuilder.
	 *
	 * @param sceneView The scene data for this frame.
	 */
	void Setup(const SceneView& sceneView);

	/**
	 * @brief Compile the frame graph.
	 *
	 * MVP: No-op.
	 * Future: Analyze resource dependencies, insert barriers, optimize ordering.
	 */
	void Compile();

	/**
	 * @brief Execute all passes.
	 *
	 * Calls Execute() on each registered pass in order, allowing them
	 * to record GPU commands via the RenderContext.
	 *
	 * @param context The render context for recording commands.
	 */
	void Execute(RenderContext& context);

	/**
	 * @brief Get the number of registered passes.
	 */
	[[nodiscard]] size_t GetPassCount() const { return m_passes.size(); }

	/**
	 * @brief Get the swap chain (for passes that need back buffer access).
	 */
	[[nodiscard]] D3D12SwapChain* GetSwapChain() const { return m_swapChain; }

	/**
	 * @brief Get the depth stencil buffer.
	 */
	[[nodiscard]] D3D12DepthStencil* GetDepthStencil() const { return m_depthStencil; }

  private:
	std::vector<std::unique_ptr<RenderPass>> m_passes;
	D3D12SwapChain* m_swapChain = nullptr;
	D3D12DepthStencil* m_depthStencil = nullptr;
	PassBuilder m_builder;
};
