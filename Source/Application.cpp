
#include "D3D12/Renderer.h"
#include "D3D12/Window.h"

int main()
{
	ThrowIfFailed(GWindow.Initialize(), "Failed To Initialize Window");

	ThrowIfFailed(GRHI.Initialize(), "Failed To Initialize RHI");

	GRenderer.Load();

	while (!GWindow.ShouldClose())
	{
		GWindow.Update();
		GRenderer.OnRender();
	}
	
	GRenderer.Shutdown();

	return 0;
}