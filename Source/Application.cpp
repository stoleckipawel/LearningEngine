
#include "D3D12/D3D12Window.h"
#include "D3D12/D3D12Renderer.h"

int main()
{
	D3D12Renderer renderer;

	if(renderer.Initialize())
	{		
		renderer.Load();

		while (!D3D12Window::Get().GetShouldClose())
		{
			D3D12Window::Get().Update();
			renderer.Render();
		}

		renderer.Shutdown();
	}	
}