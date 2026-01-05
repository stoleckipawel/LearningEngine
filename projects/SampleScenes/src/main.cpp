#include "App.h"

// SampleScenes app demonstrating engine lifecycle hooks.
// Override Pre/Post hooks to extend behavior without replacing engine steps.
class SampleScenes : public App
{
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

// Application entry point.
int main()
{
	SampleScenes app;
	app.Run();
	return 0;
}
