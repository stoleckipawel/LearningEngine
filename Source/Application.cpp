
#include "D3D12/Renderer.h"
#include "D3D12/Window.h"

int main()
{
	if(GRenderer.Initialize())
	{		
		GRenderer.Load();

		while (!GWindow.ShouldClose())
		{
			GWindow.Update();

			GRenderer.OnRender();
		}
		GRenderer.Shutdown();
	}	

	return 0;
}