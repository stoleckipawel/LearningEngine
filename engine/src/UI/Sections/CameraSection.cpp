#include "PCH.h"
#include "CameraSection.h"
#include "DepthConvention.h"
#include "Camera.h"

#include <imgui.h>

void CameraSection::BuildUI()
{
    // Field of View
    float fovDegrees = GCamera.GetFovYDegrees();
    ImGui::Text("FOV (Vertical)");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::SliderFloat("##FOV", &fovDegrees, 30.0f, 120.0f, "%.1f deg"))
    {
        GCamera.SetFovYDegrees(fovDegrees);
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // Near/Far plane controls
    float nearZ = GCamera.GetNearZ();
    float farZ = GCamera.GetFarZ();

    ImGui::Text("Near Plane");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::DragFloat("##NearZ", &nearZ, 0.001f, 0.001f, farZ - 0.01f, "%.3f"))
    {
        GCamera.SetNearFar(nearZ, farZ);
    }
    ImGui::PopItemWidth();

    ImGui::Text("Far Plane");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::DragFloat("##FarZ", &farZ, 10.0f, nearZ + 0.01f, 1000000.0f, "%.0f"))
    {
        GCamera.SetNearFar(nearZ, farZ);
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // Depth convention mode
    DepthMode currentMode = DepthConvention::GetMode();
    int modeIndex = static_cast<int>(currentMode);

    const char* modeNames[] = {"Standard (Near=0, Far=1)", "Reversed-Z (Near=1, Far=0)"};

    ImGui::Text("Depth Mode");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::Combo("##DepthMode", &modeIndex, modeNames, IM_ARRAYSIZE(modeNames)))
    {
        DepthConvention::SetMode(static_cast<DepthMode>(modeIndex));
    }
    ImGui::PopItemWidth();
}
