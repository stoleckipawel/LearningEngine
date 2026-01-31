#include "PCH.h"
#include "TimeControls.h"

#include "Timer.h"

#include <imgui.h>

TimeControls::TimeControls(Timer& timer) noexcept : m_timer(timer) {}

void TimeControls::BuildUI()
{
	float timeScale = static_cast<float>(m_timer.GetTimeScale());
	if (ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 4.0f, "%.2f"))
	{
		m_timer.SetTimeScale(static_cast<double>(timeScale));
	}

	bool paused = m_timer.IsPaused();
	if (ImGui::Checkbox("Paused", &paused))
	{
		if (paused)
			m_timer.Pause();
		else
			m_timer.Resume();
	}
}
