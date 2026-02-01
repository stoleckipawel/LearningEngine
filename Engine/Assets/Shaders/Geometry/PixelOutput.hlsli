#pragma once

// =============================================================================
// Pixel Shader Output Structure
// =============================================================================
// Render target outputs

namespace PS
{
	struct Output
	{
		float4 Color0 : SV_Target0;
	};
}  // namespace PS
