#pragma once

#include "Renderer/Public/RendererAPI.h"
#include "Renderer/Public/FrameGraph/ResourceHandle.h"
#include "Renderer/Public/FrameGraph/ResourceState.h"

/**
 * @brief Builder interface for render passes to declare resource usage.
 *
 * PassBuilder is passed to RenderPass::Setup() to allow passes to declare
 * which resources they read from and write to. This information is used
 * by the FrameGraph to manage resource lifetimes and state transitions.
 *
 * MVP Implementation: Only UseBackBuffer() and UseDepthBuffer() are functional.
 * Future: Read(), Write(), CreateTexture() for transient resources and automatic barriers.
 */
class SPARKLE_RENDERER_API PassBuilder
{
public:
	PassBuilder() = default;
	~PassBuilder() = default;

	// Non-copyable, non-movable
	PassBuilder(const PassBuilder&) = delete;
	PassBuilder& operator=(const PassBuilder&) = delete;
	PassBuilder(PassBuilder&&) = delete;
	PassBuilder& operator=(PassBuilder&&) = delete;

	/**
	 * @brief Declare write access to the back buffer.
	 * @return Handle to the back buffer resource.
	 */
	ResourceHandle UseBackBuffer()
	{
		return ResourceHandle::BackBuffer();
	}

	/**
	 * @brief Declare write access to the depth buffer.
	 * @return Handle to the depth buffer resource.
	 */
	ResourceHandle UseDepthBuffer()
	{
		return ResourceHandle::DepthBuffer();
	}

	// ─────────────────────────────────────────────────────────────────────────
	// Future API (stubbed for MVP)
	// ─────────────────────────────────────────────────────────────────────────

	/**
	 * @brief Declare read access to a resource. (Future)
	 * @param handle The resource to read from.
	 * @param state The state the resource should be in for reading.
	 * @return The same handle for chaining.
	 */
	ResourceHandle Read([[maybe_unused]] ResourceHandle handle, [[maybe_unused]] ResourceState state)
	{
		// MVP: No-op, just return the handle
		return handle;
	}

	/**
	 * @brief Declare write access to a resource. (Future)
	 * @param handle The resource to write to.
	 * @param state The state the resource should be in for writing.
	 * @return The same handle for chaining.
	 */
	ResourceHandle Write([[maybe_unused]] ResourceHandle handle, [[maybe_unused]] ResourceState state)
	{
		// MVP: No-op, just return the handle
		return handle;
	}

	/**
	 * @brief Create a transient texture resource. (Future)
	 * @param desc Texture description (placeholder type for now).
	 * @return Handle to the created transient texture.
	 */
	template<typename TextureDesc>
	ResourceHandle CreateTexture([[maybe_unused]] const TextureDesc& desc)
	{
		// MVP: Return invalid handle - transient resources not yet supported
		return ResourceHandle::Invalid();
	}
};
