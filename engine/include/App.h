#pragma once

// Application base using the Non-Virtual Interface (NVI) pattern.
// Engine-owned public methods enforce sequencing and call user hooks
// before and after the engine's main work, enabling extension without replacement.
class App {
public:
    // Engine-controlled lifecycle (do not override)
    // Calls Pre*, engine work, then Post* in that order.
    // Entry point to run the application (only public API needed by users)
    void Run();

protected:
    // Prevent direct instantiation: only derived classes can construct
    App() = default;

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
    // Engine-controlled lifecycle (not accessible to user code directly)
    void Initialize();
    void Render();
    void Resize();
    void Shutdown();
};
