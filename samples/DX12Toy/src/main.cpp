
#include "PCH.h"
#include "Core/Window.h"
#include "Core/Renderer.h"

// Entry point for the application
int main()
{
	// Initialize the Windows window
	GWindow.Initialize();

	// Initialize the rendering hardware interface (RHI)
	GRHI.Initialize();

	// Load renderer resources (geometry, shaders, etc.)
	GRenderer.Load();

	// Main application loop
	while (!GWindow.ShouldClose())
	{
		// Handle window events and updates
		GWindow.Update();
		// Render the current frame
		GRenderer.OnRender();
	}

	// Shutdown and release renderer resources
	GRenderer.Shutdown();
	
	return 0;
}