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
	// Use DirectXMath's sin/cos for precision (matches XMMatrixPerspectiveFovLH)
	float sinFov, cosFov;
	XMScalarSinCos(&sinFov, &cosFov, 0.5f * fovY);

	const float height = cosFov / sinFov;  // cot(fov/2)
	const float width = height / aspect;

	if (IsReversedZ())
	{
		// Reversed-Z: near → 1, far → 0
		const float fRange = nearZ / (nearZ - farZ);

		XMMATRIX m;
		m.r[0] = XMVectorSet(width, 0.0f, 0.0f, 0.0f);
		m.r[1] = XMVectorSet(0.0f, height, 0.0f, 0.0f);
		m.r[2] = XMVectorSet(0.0f, 0.0f, fRange, 1.0f);
		m.r[3] = XMVectorSet(0.0f, 0.0f, -fRange * farZ, 0.0f);
		return m;
	}

	// Standard: near → 0, far → 1
	return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
}

//------------------------------------------------------------------------------
// Depth Linearization
//------------------------------------------------------------------------------

float DepthConvention::LinearizeDepth(float ndcDepth, float nearZ, float farZ) noexcept
{
	if (IsReversedZ())
	{
		if (ndcDepth <= 0.0f)
			return farZ;
		return nearZ / ndcDepth;
	}

	// Standard: linearZ = near * far / (far - ndc * (far - near))
	const float range = farZ - nearZ;
	return (nearZ * farZ) / (farZ - ndcDepth * range);
}

float DepthConvention::DepthToNDC(float linearZ, float nearZ, float farZ) noexcept
{
	if (IsReversedZ())
	{
		if (linearZ <= 0.0f)
			return 0.0f;
		return nearZ / linearZ;
	}

	// Standard: ndc = (far * (z - near)) / (z * (far - near))
	const float range = farZ - nearZ;
	return (farZ * (linearZ - nearZ)) / (linearZ * range);
}
