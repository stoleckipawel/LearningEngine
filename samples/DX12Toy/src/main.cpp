
#include "Core/PCH.h"
#include "Core/Window.h"
#include "Core/Renderer.h"

// Entry point for the application
int main()
{
	// Initialize the Windows window
	GWindow.Initialize();

	GRenderer.Initialize();

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