#include "PCH.h"
#include "ViewMode.h"

#include <imgui.h>

static constexpr const char* kViewModeNames[] = {
	"Lit",
	"GBuffer Diffuse",
	"GBuffer Normal",
	"GBuffer Roughness",
	"GBuffer Metallic",
	"GBuffer Emissive",
};

void ViewMode::BuildUI()
{
	int modeIndex = static_cast<int>(m_mode);
	if (ImGui::Combo(
	        "View Mode",
	        &modeIndex,
	        kViewModeNames,
	        static_cast<int>(sizeof(kViewModeNames) / sizeof(kViewModeNames[0]))))
	{
		m_mode = static_cast<Id>(modeIndex);
	}
}
