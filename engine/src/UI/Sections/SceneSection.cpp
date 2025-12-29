#include "PCH.h"
#include "SceneSection.h"
#include "Scene/Scene.h"

#include <imgui.h>

static constexpr const char* kShapeNames[] = {
    "Box",
    "Plane",
    "Sphere",
    "Cone",
    "Cylinder",
    "Torus",
    "Capsule",
    "Hemisphere",
    "Pyramid",
    "Disk",
    "Octahedron",
    "Tetrahedron",
    "Icosahedron",
    "Dodecahedron",
    "Icosphere",
};

static_assert(
    sizeof(kShapeNames) / sizeof(kShapeNames[0]) == static_cast<std::size_t>(MeshFactory::Shape::Icosphere) + 1,
    "Shape names must match MeshFactory::Shape enum");

SceneSection::SceneSection() noexcept
{
	// Initialize from current scene state
	m_shapeIndex = static_cast<int>(GScene.GetCurrentShape());
	m_count = static_cast<int>(GScene.GetCurrentCount());
}

void SceneSection::BuildUI()
{
	bool changed = false;

	if (ImGui::Combo("Primitive", &m_shapeIndex, kShapeNames, static_cast<int>(sizeof(kShapeNames) / sizeof(kShapeNames[0]))))
	{
		changed = true;
	}

	if (ImGui::SliderInt("Count", &m_count, 1, 512))
	{
		changed = true;
	}

	if (changed)
	{
		GScene.SetPrimitives(static_cast<MeshFactory::Shape>(m_shapeIndex), static_cast<std::uint32_t>(m_count));
	}
}
