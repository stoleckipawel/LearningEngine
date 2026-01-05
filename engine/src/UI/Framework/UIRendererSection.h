// ============================================================================
// UIRendererSection.h
// ----------------------------------------------------------------------------
// Base interface and IDs for sections within the renderer settings panel.
//
// USAGE:
//   class MySection : public UIRendererSection {
//       UIRendererSectionId GetId() const noexcept override { return UIRendererSectionId::Stats; }
//       const char* GetTitle() const noexcept override { return "My Section"; }
//       void BuildUI() override { /* ImGui calls */ }
//   };
//
// DESIGN:
//   - Stable IDs allow typed retrieval and replacement of sections
//   - Sections are owned by RendererPanel and are long-lived
//
// NOTES:
//   - Add new section IDs before Count
//   - BuildUI() is called each frame when section is visible
// ============================================================================

#pragma once

#include <cstdint>

// ============================================================================
// Section Identifiers
// ============================================================================

/// Stable IDs for sections rendered inside the renderer panel.
/// Used for replacement, ordering, and typed retrieval.
enum class UIRendererSectionId : std::uint8_t
{
	Stats = 0,  ///< Performance statistics (FPS, frame time)
	ViewMode,   ///< Render view mode selection (lit, wireframe, etc.)
	Time,       ///< Time control (speed, pause)
	Scene,      ///< Scene configuration (primitives, etc.)
	Camera,     ///< Camera settings (FOV, position)
	Count       ///< Number of section IDs (for array sizing)
};

// ============================================================================
// UIRendererSection Interface
// ============================================================================

/// Base interface for a renderable section inside the renderer panel.
/// Sections are owned by the panel and are expected to be long-lived.
class UIRendererSection
{
  public:
	virtual ~UIRendererSection() = default;

	/// Returns the stable ID for this section type.
	virtual UIRendererSectionId GetId() const noexcept = 0;

	/// Returns the display title for the collapsing header.
	virtual const char* GetTitle() const noexcept = 0;

	/// Called each frame to build ImGui content for this section.
	virtual void BuildUI() = 0;
};
