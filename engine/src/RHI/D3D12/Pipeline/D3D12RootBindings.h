#pragma once

#include <cstdint>
#include <d3d12.h>

//------------------------------------------------------------------------------
// RootBindings.h - Single Source of Truth for Shader Resource Binding Layout
//------------------------------------------------------------------------------
// This header defines the canonical binding slots for the engine's root signature.
// It MUST be kept in sync with:
//   - RootSignature.cpp (C++ root signature creation)
//   - ConstantBufferData.hlsli (HLSL register declarations)
//
// Design Philosophy:
//   - Constant buffers use root CBVs (direct GPU VA binding) for low overhead
//   - Textures/Samplers use descriptor tables for flexibility
//   - Binding frequency drives slot assignment (per-frame → per-object)
//
// Update Frequency (binding cost optimization):
//   - Slots 0-1: Bound once per frame (low frequency)
//   - Slots 2-3: Bound per draw call (high frequency)
//   - Slots 4-5: Bound once per frame or per-material (medium frequency)
//------------------------------------------------------------------------------

namespace RootBindings
{
	//--------------------------------------------------------------------------
	// Root Parameter Indices (D3D12 root signature parameter slots)
	//--------------------------------------------------------------------------
	// These indices are passed to SetGraphicsRootConstantBufferView() and
	// SetGraphicsRootDescriptorTable() to bind resources to the pipeline.
	//--------------------------------------------------------------------------

	namespace RootParam
	{
		// Constant Buffer Views (root CBVs - direct GPU virtual address binding)
		constexpr uint32_t PerFrame = 0;     // b0 - Per-frame data (time, viewport)
		constexpr uint32_t PerView = 1;      // b1 - Per-view/camera data (matrices)
		constexpr uint32_t PerObjectVS = 2;  // b2 - Per-object vertex shader data (world matrix)
		constexpr uint32_t PerObjectPS = 3;  // b3 - Per-object pixel shader data (material)

		// Descriptor Tables (heap-based binding)
		constexpr uint32_t TextureSRV = 4;  // t0+ - Texture SRVs
		constexpr uint32_t Sampler = 5;     // s0+ - Samplers

		// Total root parameter count (for validation)
		constexpr uint32_t Count = 6;
	}  // namespace RootParam

	//--------------------------------------------------------------------------
	// HLSL Register Slots (constant buffer register assignments)
	//--------------------------------------------------------------------------
	// These match the register(bN) declarations in HLSL shaders.
	// They are separate from RootParam indices when using descriptor tables,
	// but identical when using root CBVs (our current design).
	//--------------------------------------------------------------------------

	namespace CBRegister
	{
		constexpr uint32_t PerFrame = 0;     // register(b0)
		constexpr uint32_t PerView = 1;      // register(b1)
		constexpr uint32_t PerObjectVS = 2;  // register(b2)
		constexpr uint32_t PerObjectPS = 3;  // register(b3)
	}  // namespace CBRegister

	namespace SRVRegister
	{
		constexpr uint32_t BaseTexture = 0;  // register(t0) - Albedo/diffuse
		                                     // Future: Normal, Roughness, Metallic, etc.
	}  // namespace SRVRegister

	namespace SamplerRegister
	{
		constexpr uint32_t LinearWrap = 0;  // register(s0)
		                                    // Future: Point, Aniso, Shadow comparison, etc.
	}  // namespace SamplerRegister

	//--------------------------------------------------------------------------
	// Shader Visibility Helpers (for documentation and validation)
	//--------------------------------------------------------------------------
	// Indicates which shader stages can access each resource.
	// Used when creating root signature parameters.
	//--------------------------------------------------------------------------

	namespace Visibility
	{
		constexpr D3D12_SHADER_VISIBILITY PerFrame = D3D12_SHADER_VISIBILITY_ALL;
		constexpr D3D12_SHADER_VISIBILITY PerView = D3D12_SHADER_VISIBILITY_ALL;
		constexpr D3D12_SHADER_VISIBILITY PerObjectVS = D3D12_SHADER_VISIBILITY_VERTEX;
		constexpr D3D12_SHADER_VISIBILITY PerObjectPS = D3D12_SHADER_VISIBILITY_PIXEL;
		constexpr D3D12_SHADER_VISIBILITY TextureSRV = D3D12_SHADER_VISIBILITY_PIXEL;
		constexpr D3D12_SHADER_VISIBILITY Sampler = D3D12_SHADER_VISIBILITY_PIXEL;
	}  // namespace Visibility

}  // namespace RootBindings