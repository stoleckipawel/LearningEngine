#pragma once

#include <cstdint>

// Stable IDs for sections rendered inside the renderer panel.
// These are used for replacement and typed retrieval.
enum class UIRendererSectionId : std::uint8_t
{
	Stats = 0,
	ViewMode,
	Time,
	Scene,
	Camera,
	Count
};

// Base interface for a renderable section inside the renderer panel.
// Sections are owned by the panel and are expected to be long-lived.
class UIRendererSection
{
  public:
	virtual ~UIRendererSection() = default;

	virtual UIRendererSectionId GetId() const noexcept = 0;
	virtual const char* GetTitle() const noexcept = 0;

	virtual void BuildUI() = 0;
};
