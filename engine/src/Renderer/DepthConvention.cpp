#include "PCH.h"
#include "DepthConvention.h"

#include <cmath>

using namespace DirectX;

//------------------------------------------------------------------------------
// Static Member Initialization
//------------------------------------------------------------------------------

DepthMode DepthConvention::s_mode = DepthMode::ReversedZ;
DepthConvention::OnModeChangedEvent DepthConvention::OnModeChanged;

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------

void DepthConvention::SetMode(DepthMode mode) noexcept
{
	if (s_mode == mode)
		return;

	s_mode = mode;
	OnModeChanged.Broadcast(mode);
}

DepthMode DepthConvention::GetMode() noexcept
{
	return s_mode;
}

bool DepthConvention::IsReversedZ() noexcept
{
	return s_mode == DepthMode::ReversedZ;
}

//------------------------------------------------------------------------------
// Depth Buffer Configuration
//------------------------------------------------------------------------------

float DepthConvention::GetClearDepth() noexcept
{
	// Reversed-Z clears to 0 (far), Standard clears to 1 (far)
	return IsReversedZ() ? 0.0f : 1.0f;
}

D3D12_COMPARISON_FUNC DepthConvention::GetDepthComparisonLessEqualFunc() noexcept
{
	// Reversed-Z: closer pixels have GREATER depth values
	// Standard:   closer pixels have LESS depth values
	return IsReversedZ() ? D3D12_COMPARISON_FUNC_GREATER : D3D12_COMPARISON_FUNC_LESS;
}

D3D12_COMPARISON_FUNC DepthConvention::GetDepthComparisonFuncEqual() noexcept
{
	return IsReversedZ() ? D3D12_COMPARISON_FUNC_GREATER_EQUAL : D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

//------------------------------------------------------------------------------
// Projection Matrix Generation
//------------------------------------------------------------------------------

XMMATRIX DepthConvention::CreatePerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ) noexcept
{
	const float h = 1.0f / std::tan(fovY * 0.5f);
	const float w = h / aspect;

	if (IsReversedZ())
	{
		// Reversed-Z: near → 1, far → 0
		//
		// DirectX LH projection matrix layout (row-major, matches XMMatrixPerspectiveFovLH):
		//   w    0    0    0
		//   0    h    0    0
		//   0    0    A    1     ← 1 copies z_view into w_clip for perspective divide
		//   0    0    B    0
		//
		// Result: x_clip = x*w, y_clip = y*h, z_clip = z*A + B, w_clip = z
		// After divide: z_ndc = A + B/z
		//
		// For reversed-Z (near→1, far→0):
		//   A + B/near = 1
		//   A + B/far  = 0
		// Solving: A = near/(near-far), B = near*far/(far-near)
		const float a = nearZ / (nearZ - farZ);
		const float b = (nearZ * farZ) / (farZ - nearZ);

		return XMMATRIX(w, 0.0f, 0.0f, 0.0f, 0.0f, h, 0.0f, 0.0f, 0.0f, 0.0f, a, 1.0f, 0.0f, 0.0f, b, 0.0f);
	}
	else
	{
		// Standard: near → 0, far → 1
		return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	}
}

//------------------------------------------------------------------------------
// Depth Linearization
//------------------------------------------------------------------------------

float DepthConvention::LinearizeDepth(float ndcDepth, float nearZ, float farZ) noexcept
{
	if (IsReversedZ())
	{
		// Reversed-Z: ndc = (far * near) / (far - z * (far - near)) ... rearranged
		// For infinite far with reversed-Z: linearZ = nearZ / ndcDepth
		if (ndcDepth <= 0.0f)
			return farZ;  // At or beyond far plane
		return nearZ / ndcDepth;
	}
	else
	{
		// Standard: linearZ = near * far / (far - ndc * (far - near))
		const float range = farZ - nearZ;
		return (nearZ * farZ) / (farZ - ndcDepth * range);
	}
}

float DepthConvention::DepthToNDC(float linearZ, float nearZ, float farZ) noexcept
{
	if (IsReversedZ())
	{
		// Reversed-Z: ndc = nearZ / linearZ
		if (linearZ <= 0.0f)
			return 0.0f;
		return nearZ / linearZ;
	}
	else
	{
		// Standard: ndc = (far * (z - near)) / (z * (far - near))
		const float range = farZ - nearZ;
		return (farZ * (linearZ - nearZ)) / (linearZ * range);
	}
}
