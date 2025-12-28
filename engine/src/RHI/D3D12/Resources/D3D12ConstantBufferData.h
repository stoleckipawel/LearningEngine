#pragma once
#include <DirectXMath.h>
#include <cstdint>
#include <type_traits>


//------------------------------------------------------------------------------
// Constant Buffer Data (C++ ↔ HLSL)
//
// Notes:
//  - These types are mirrored in HLSL (`Common.hlsli` keep parity).
//  - Constant buffers (CBV) in D3D12 expect views and offsets to be 256-byte
//    aligned; therefore types bound as CBVs are defined with alignas(256).
//  - Structured buffers (SRV) used for instance data do not require 256-byte
//    per-element alignment — they follow HLSL packing (16-byte / float4 lanes).
//
// HLSL register conventions (example):
//   b0 -> PerFrameConstantBufferData
//   b1 -> PerViewConstantBufferData
//   b2 -> PerObjectVSConstantBufferData
//   b3 -> PerObjectPSConstantBufferData
//------------------------------------------------------------------------------

// Helper macro to validate CBV layout/size for all constant-buffer structs.
// Usage: CBV_CHECK(MyCbStruct);
// Validate that CB types are safe to memcpy to GPU memory and match HLSL layout
#define CBV_CHECK(Type)                                                                      \
	static_assert(std::is_standard_layout_v<Type>, #Type " must be standard-layout");        \
	static_assert(std::is_trivially_copyable_v<Type>, #Type " must be trivially-copyable");  \
	static_assert(alignof(Type) >= 256, #Type " must be 256-byte aligned");                  \
	static_assert(sizeof(Type) % 256 == 0, #Type " must occupy whole 256-byte CBV slot(s)"); \
	static_assert(sizeof(Type) <= 64 * 1024, #Type " must be <= 64KB")

//------------------------------------------------------------------------------
// Per-Frame CB (b0) — updated once per CPU frame, shared by all draws
//------------------------------------------------------------------------------
struct alignas(256) PerFrameConstantBufferData
{
	uint32_t FrameIndex;     // Current frame number
	float TotalTime;         // Seconds since engine start
	float DeltaTime;         // Seconds since last frame
	uint32_t ViewModeIndex;  // Current renderer debug view mode

	DirectX::XMFLOAT2 ViewportSize;     // Render target width, height
	DirectX::XMFLOAT2 ViewportSizeInv;  // 1.0 / width, 1.0 / height

	// rest of 256-byte slot is intentionally unused/pad
};
CBV_CHECK(PerFrameConstantBufferData);

//------------------------------------------------------------------------------
// Per-View CB (b1) — updated per camera/view (main, shadow, reflection, etc.)
//------------------------------------------------------------------------------
struct alignas(256) PerViewConstantBufferData
{
	DirectX::XMFLOAT4X4 ViewMTX;        // World -> View
	DirectX::XMFLOAT4X4 ProjectionMTX;  // View -> Clip
	DirectX::XMFLOAT4X4 ViewProjMTX;    // World -> Clip (precomputed to save GPU work)

	DirectX::XMFLOAT3 CameraPosition;  // World-space camera position
	float NearZ;                       // Near clip plane

	float FarZ;                         // Far clip plane
	DirectX::XMFLOAT3 CameraDirection;  // World-space camera forward
};
CBV_CHECK(PerViewConstantBufferData);

//------------------------------------------------------------------------------
// Per-Object VS CB (b2) — updated per draw call (transforms)
//------------------------------------------------------------------------------
struct alignas(256) PerObjectVSConstantBufferData
{
	DirectX::XMFLOAT4X4 WorldMTX;              // Local -> World
	DirectX::XMFLOAT3X4 WorldInvTransposeMTX;  // Normal matrix (inverse-transpose, 3x4 = 48 bytes matches HLSL float3x3 cbuffer packing)

	// remaining space in the 256-byte slot is reserved for future use
};
CBV_CHECK(PerObjectVSConstantBufferData);
//------------------------------------------------------------------------------
// Per-Object PS CB (b3) — updated per draw call (material scalars)
//------------------------------------------------------------------------------
struct alignas(256) PerObjectPSConstantBufferData
{
	DirectX::XMFLOAT4 BaseColor;  // RGBA base/albedo color or tint

	float Metallic;          // PBR metallic [0,1]
	float Roughness;         // PBR roughness [0,1]
	float F0;                // PBR reflectance at normal incidence
	float _padPerObjectPS0;  // pad to 16 bytes

	// remaining space reserved
};
CBV_CHECK(PerObjectPSConstantBufferData);

//------------------------------------------------------------------------------
// ToDo: Per-Instance Data (structured buffer element) — updated per instance