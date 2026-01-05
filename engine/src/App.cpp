#include "PCH.h"
#include "App.h"
#include "UI.h"
#include "Window.h"
#include "Renderer.h"
#include "Assets/AssetSystem.h"

#include <utility>

App::App(std::string windowTitle)
	: m_windowTitle(std::move(windowTitle))
{
}

void App::Run()
{
	// Initialize platform window + renderer
	Initialize();

	// Main loop
	RenderLoop();

	// Shutdown engine
	Shutdown();
}

void App::RenderLoop()
{
	while (!GWindow.ShouldClose())
	{
		// Engine input & message pump
		GWindow.PollEvents();

		// Renders the scene
		Render();
	}
}

// --- NVI implementations ---
void App::Initialize()
{
	// User can inject work before engine init
	PreInitialize();

	// Initialize asset system
	GAssetSystem.Initialize();

	// Engine base init
	GWindow.Initialize(GetWindowTitle());
	GRenderer.Initialize();

	// User can extend after engine init
	PostInitialize();
}

void App::Render()
{
	// User can record/prepare before engine render
	PreRender();

	// Engine render frame
	GRenderer.OnRender();

	// User can extend after engine render
	PostRender();
}

void App::Resize()
{
	// User hook before engine resize
	PreResize();

	// Engine resize
	GRenderer.OnResize();

	// User hook after engine resize
	PostResize();
}

void App::Shutdown()
{
	// User hook before engine shutdown
	PreShutdown();

	// Engine shutdown
	GRenderer.Shutdown();

	// Shutdown asset system
	GAssetSystem.Shutdown();

	// User hook after engine shutdown
	PostShutdown();
}
