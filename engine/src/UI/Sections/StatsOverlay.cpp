#include "PCH.h"
#include "StatsOverlay.h"

#include "Timer.h"
#include <imgui.h>

void StatsOverlay::BuildUI()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("FPS: %.1f", io.Framerate);
	ImGui::Text("FrameTime: %.2f ms", io.DeltaTime * 1000.0f);
	ImGui::Text("FrameIndex: %llu", static_cast<unsigned long long>(GTimer.GetFrameCount()));
}