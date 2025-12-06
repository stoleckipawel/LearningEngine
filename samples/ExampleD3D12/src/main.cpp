#include "PCH.h"
#include "App.h"

// Example D3D12 app demonstrating engine lifecycle hooks.
// Override Pre/Post hooks to extend behavior without replacing engine steps.
class ExampleD3D12 : public App {
protected:
	// Startup: load content, register systems, etc.
	void PreInitialize() override {}
	void PostInitialize() override {}

	// Rendering: add debug overlays or extra passes pre/post engine render.
	void PreRender() override {}
	void PostRender() override {}

	// Shutdown: release app-owned resources.
	void PreShutdown() override {}
	void PostShutdown() override {}
};

// Entry point for the application
// Application entry point: construct app and hand control to engine.
int main() {
	ExampleD3D12 app;
	app.Run();
	return 0;
}