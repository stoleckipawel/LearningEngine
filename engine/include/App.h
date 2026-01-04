#pragma once

// Application base using the Non-Virtual Interface (NVI) pattern.
// Engine-owned public methods enforce sequencing and call user hooks
// before and after the engine's main work, enabling extension without replacement.
class App
{
  public:
	App() = default;
	~App() = default;

	// Entry point to run the application.
	void Run();

  protected:
	// User extension points (override as needed). All are optional.

	// Initialize: create gameplay state, load resources, etc.
	virtual void PreInitialize() {}
	virtual void PostInitialize() {}

	// Render: additional scene recording or debug overlays.
	virtual void PreRender() {}
	virtual void PostRender() {}

	// Resize: react to window size changes.
	virtual void PreResize() {}
	virtual void PostResize() {}

	// Shutdown: cleanup gameplay resources.
	virtual void PreShutdown() {}
	virtual void PostShutdown() {}

	// Encapsulated per-frame main loop; called by Run().
	void RenderLoop();

  private:
	// Engine-controlled lifecycle
	void Initialize();
	void Render();
	void Resize();
	void Shutdown();
};
