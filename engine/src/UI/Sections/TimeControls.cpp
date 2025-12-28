#include "PCH.h"
#include "TimeControls.h"

#include "Timer.h"

#include <imgui.h>

void TimeControls::BuildUI()
{
	float timeScale = static_cast<float>(GTimer.GetTimeScale());
	if (ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 4.0f, "%.2f"))
	{
		GTimer.SetTimeScale(static_cast<double>(timeScale));
	}

	bool paused = GTimer.IsPaused();
	if (ImGui::Checkbox("Paused", &paused))
	{
		if (paused)
			GTimer.Pause();
		else
			GTimer.Resume();
	}
}
