# Option B: Federated Static Libraries (Godot-Style)

**Reference Engines:** Godot Engine, Hazel Engine

---

## Overview

The Federated Static Libraries architecture uses the "Server Pattern" where abstract singleton servers manage subsystems, with all code compiled into a single executable. This provides clean abstractions without DLL complexity.

```
                              ┌─────────────┐
                              │   main.cpp  │
                              │  (sparkle)  │
                              └──────┬──────┘
                                     │
        ┌────────────────────────────┼────────────────────────────┐
        │                            │                            │
        ▼                            ▼                            ▼
┌───────────────┐          ┌─────────────────┐          ┌───────────────┐
│    editor/    │          │    modules/     │          │   platform/   │
│ (optional)    │          │ (gltf, recast)  │          │  (windows)    │
└───────┬───────┘          └────────┬────────┘          └───────┬───────┘
        │                           │                           │
        └───────────────────────────┼───────────────────────────┘
                                    │
                                    ▼
                           ┌─────────────────┐
                           │     scene/      │
                           │ (Nodes, GUI,    │
                           │  Resources)     │
                           └────────┬────────┘
                                    │
                   ┌────────────────┼────────────────┐
                   │                │                │
                   ▼                ▼                ▼
          ┌───────────────┐ ┌─────────────┐ ┌───────────────┐
          │   servers/    │ │  drivers/   │ │   servers/    │
          │  rendering/   │ │   d3d12/    │ │   physics/    │
          └───────┬───────┘ └──────┬──────┘ └───────┬───────┘
                  │                │                │
                  └────────────────┼────────────────┘
                                   │
                                   ▼
                           ┌───────────────┐
                           │     core/     │
                           │ (math, memory,│
                           │  variant)     │
                           └───────────────┘

═══════════════════════════════════════════════════════════════
               ALL STATIC LINKING → Single sparkle.exe
═══════════════════════════════════════════════════════════════
```

---

## Directory Structure

```
SparkleEngine/
│
├── core/                              ══════════════════════════════╗
│   ├── core/                          ║ libsparkle_core.a           ║
│   │   ├── math/                      ║ ────────────────────────    ║
│   │   │   ├── vector3.h              ║ Foundation types only       ║
│   │   │   ├── vector3.cpp            ║ No dependencies             ║
│   │   │   ├── transform.h            ║                             ║
│   │   │   └── aabb.h                 ║ Very stable, rarely changes ║
│   │   ├── memory/                    ╚══════════════════════════════╝
│   │   │   ├── memory.h
│   │   │   └── allocator.cpp
│   │   ├── string/
│   │   │   └── string_name.h
│   │   ├── variant/                   # Dynamic typing system
│   │   │   ├── variant.h
│   │   │   └── variant.cpp
│   │   └── object/                    # Base object with signals
│   │       ├── object.h
│   │       └── object.cpp
│   └── CMakeLists.txt
│
├── servers/                           ══════════════════════════════╗
│   │                                  ║ Server Pattern              ║
│   │                                  ║ ────────────────────────    ║
│   │                                  ║ Abstract singleton servers  ║
│   │                                  ║ that manage subsystems      ║
│   │                                  ╚══════════════════════════════╝
│   │
│   ├── rendering/                     ══════════════════════════════╗
│   │   ├── rendering_server.h         ║ RenderingServer             ║
│   │   ├── rendering_server.cpp       ║ ────────────────────────    ║
│   │   └── renderer/                  ║ • Abstract rendering API    ║
│   │       ├── renderer_compositor.h  ║ • Uses RID handles          ║
│   │       ├── renderer_compositor.cpp║ • Backend-agnostic          ║
│   │       ├── renderer_scene.cpp     ║                             ║
│   │       └── renderer_canvas.cpp    ║ No D3D12/Vulkan headers!    ║
│   │                                  ╚══════════════════════════════╝
│   │
│   ├── physics/                       ══════════════════════════════╗
│   │   ├── physics_server_3d.h        ║ PhysicsServer3D             ║
│   │   ├── physics_server_3d.cpp      ║ ────────────────────────    ║
│   │   └── shapes/                    ║ • Abstract physics API      ║
│   │       ├── shape_3d.h             ║ • Collision queries         ║
│   │       └── box_shape_3d.cpp       ║ • Rigid body management     ║
│   │                                  ╚══════════════════════════════╝
│   │
│   ├── audio/                         ══════════════════════════════╗
│   │   ├── audio_server.h             ║ AudioServer                 ║
│   │   └── audio_server.cpp           ║ ────────────────────────    ║
│   │                                  ║ • Sound playback            ║
│   │                                  ║ • 3D spatialization         ║
│   │                                  ╚══════════════════════════════╝
│   │
│   └── navigation/                    ══════════════════════════════╗
│       ├── navigation_server_3d.h     ║ NavigationServer3D          ║
│       └── navigation_server_3d.cpp   ║ ────────────────────────    ║
│                                      ║ • Pathfinding               ║
│                                      ║ • NavMesh queries           ║
│                                      ╚══════════════════════════════╝
│
├── scene/                             ══════════════════════════════╗
│   │                                  ║ Scene Tree Architecture     ║
│   │                                  ║ ────────────────────────    ║
│   │                                  ║ Nodes form a tree           ║
│   │                                  ║ Each node has callbacks:    ║
│   │                                  ║ • _ready()                  ║
│   │                                  ║ • _process(delta)           ║
│   │                                  ║ • _physics_process(delta)   ║
│   │                                  ╚══════════════════════════════╝
│   │
│   ├── main/
│   │   ├── scene_tree.h
│   │   ├── scene_tree.cpp
│   │   ├── viewport.h
│   │   ├── viewport.cpp
│   │   └── window.h
│   │
│   ├── 3d/
│   │   ├── node_3d.h                  # Base for all 3D nodes
│   │   ├── node_3d.cpp
│   │   ├── camera_3d.h
│   │   ├── camera_3d.cpp
│   │   ├── mesh_instance_3d.h
│   │   ├── mesh_instance_3d.cpp
│   │   ├── light_3d.h
│   │   ├── light_3d.cpp
│   │   ├── skeleton_3d.h
│   │   └── physics/
│   │       ├── rigid_body_3d.h
│   │       ├── rigid_body_3d.cpp
│   │       ├── collision_shape_3d.h
│   │       ├── static_body_3d.h
│   │       └── area_3d.h
│   │
│   ├── animation/
│   │   ├── animation_player.h
│   │   ├── animation_player.cpp
│   │   ├── animation_tree.h
│   │   └── tween.h
│   │
│   ├── gui/                           # Control nodes (2D UI)
│   │   ├── control.h
│   │   ├── control.cpp
│   │   ├── button.h
│   │   ├── label.h
│   │   └── container.h
│   │
│   └── resources/                     # Loadable resources
│       ├── mesh.h
│       ├── mesh.cpp
│       ├── material.h
│       ├── texture.h
│       └── animation.h
│
├── drivers/                           ══════════════════════════════╗
│   │                                  ║ Backend Implementations     ║
│   │                                  ║ ────────────────────────    ║
│   │                                  ║ Concrete implementations    ║
│   │                                  ║ of server interfaces        ║
│   │                                  ║                             ║
│   │                                  ║ Swappable at compile time   ║
│   │                                  ╚══════════════════════════════╝
│   │
│   ├── d3d12/
│   │   ├── rendering_device_d3d12.h
│   │   ├── rendering_device_d3d12.cpp
│   │   ├── d3d12_context.h
│   │   ├── d3d12_context.cpp
│   │   └── shaders/
│   │       └── ...
│   │
│   ├── vulkan/
│   │   ├── rendering_device_vulkan.h
│   │   ├── rendering_device_vulkan.cpp
│   │   └── ...
│   │
│   └── physx/
│       ├── physics_server_physx.h
│       └── physics_server_physx.cpp
│
├── modules/                           ══════════════════════════════╗
│   │                                  ║ Optional Modules            ║
│   │                                  ║ ────────────────────────    ║
│   │                                  ║ Can be compiled in/out      ║
│   │                                  ║ Self-registering            ║
│   │                                  ║                             ║
│   │                                  ║ Each module has:            ║
│   │                                  ║ • register_types()          ║
│   │                                  ║ • unregister_types()        ║
│   │                                  ╚══════════════════════════════╝
│   │
│   ├── gltf/
│   │   ├── gltf_document.h
│   │   ├── gltf_document.cpp
│   │   ├── gltf_mesh.cpp
│   │   └── register_types.cpp
│   │
│   ├── freetype/
│   │   └── ...
│   │
│   ├── recast/                        # NavMesh generation
│   │   └── ...
│   │
│   └── imgui_debug/
│       ├── imgui_module.h
│       └── register_types.cpp
│
├── editor/                            # Editor (separate build target)
│   ├── editor_node.h
│   ├── editor_node.cpp
│   ├── plugins/
│   │   ├── mesh_editor_plugin.cpp
│   │   └── material_editor_plugin.cpp
│   └── ...
│
├── platform/                          # Platform-specific code
│   ├── windows/
│   │   ├── os_windows.h
│   │   ├── os_windows.cpp
│   │   └── display_server_windows.cpp
│   ├── linux/
│   │   └── ...
│   └── macos/
│       └── ...
│
├── main/                              # Entry points
│   ├── main.cpp
│   └── main_loop.cpp
│
└── projects/
    └── my_game/
        ├── project.sparkle
        ├── scenes/
        │   └── main.scene
        └── scripts/
            └── player.cpp
```

---

## Key Concepts

### 1. Server Pattern

Abstract servers hide implementation details:

```cpp
// servers/rendering/rendering_server.h
class RenderingServer
{
public:
    static RenderingServer* get_singleton();
    
    // Create resources - returns opaque handle
    RID mesh_create();
    void mesh_add_surface(RID mesh, const Array& vertices);
    
    // Scene management
    RID instance_create();
    void instance_set_mesh(RID instance, RID mesh);
    void instance_set_transform(RID instance, const Transform3D& xform);
    
    // Rendering
    void draw();
    
protected:
    // Implementation in drivers/
    virtual RID _mesh_create() = 0;
};

// drivers/d3d12/rendering_device_d3d12.cpp
class RenderingServerD3D12 : public RenderingServer
{
    RID _mesh_create() override
    {
        // D3D12 implementation
        ID3D12Resource* buffer = ...;
        return make_rid(buffer);
    }
};
```

### 2. RID (Resource ID) Handles

Opaque handles prevent leaking backend types:

```cpp
// Scene code - no D3D12 includes!
class MeshInstance3D : public Node3D
{
    RID mesh_rid;
    RID instance_rid;
    
    void set_mesh(Ref<Mesh> mesh)
    {
        mesh_rid = mesh->get_rid();
        instance_rid = RS::get_singleton()->instance_create();
        RS::get_singleton()->instance_set_mesh(instance_rid, mesh_rid);
    }
};
```

### 3. Scene Tree

Nodes form a hierarchy with lifecycle callbacks:

```cpp
class Player : public CharacterBody3D
{
    void _ready() override
    {
        // Called when node enters tree
    }
    
    void _process(double delta) override
    {
        // Called every frame
    }
    
    void _physics_process(double delta) override
    {
        // Called at fixed rate
    }
};
```

### 4. Signals

Event system for decoupled communication:

```cpp
// Define signal
class Button : public Control
{
    SIGNAL(pressed);
    
    void _gui_input(InputEvent* event) override
    {
        if (event->is_pressed())
            emit_signal("pressed");
    }
};

// Connect signal
button->connect("pressed", callable_mp(this, &MyClass::on_button_pressed));
```

---

## Build System

All modules compile to static libraries, then link into single executable:

```cmake
# core/CMakeLists.txt
add_library(sparkle_core STATIC
    core/math/vector3.cpp
    core/memory/allocator.cpp
    core/variant/variant.cpp
    core/object/object.cpp
)

# servers/CMakeLists.txt
add_library(sparkle_servers STATIC
    rendering/rendering_server.cpp
    physics/physics_server_3d.cpp
    audio/audio_server.cpp
)
target_link_libraries(sparkle_servers sparkle_core)

# scene/CMakeLists.txt
add_library(sparkle_scene STATIC
    main/scene_tree.cpp
    3d/node_3d.cpp
    3d/mesh_instance_3d.cpp
    gui/control.cpp
)
target_link_libraries(sparkle_scene sparkle_servers sparkle_core)

# Final executable
add_executable(sparkle main/main.cpp)
target_link_libraries(sparkle
    sparkle_scene
    sparkle_servers
    sparkle_drivers_d3d12
    sparkle_modules
    sparkle_core
)
```

---

## Build Impact Analysis

```
┌─────────────────────┬───────────────────────────────────────────────────────┐
│ File Changed        │ Impact                                                │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ core/math/vector.h  │ Recompile: ██████████████████████ Many files          │
│                     │ Relink:    ████████████████████████████████ Full      │
│                     │ Time: ~90 seconds                                     │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ scene/3d/node_3d.h  │ Recompile: ████████████ Scene files                   │
│                     │ Relink:    ████████████████████████████████ Full      │
│                     │ Time: ~60 seconds                                     │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ drivers/d3d12/*.cpp │ Recompile: ██ Just that file                          │
│                     │ Relink:    ████████████████████████████████ Full      │
│                     │ Time: ~45 seconds                                     │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ game/player.cpp     │ Recompile: █ Just that file                           │
│                     │ Relink:    ████████████████████████████████ Full      │
│                     │ Time: ~40 seconds                                     │
└─────────────────────┴───────────────────────────────────────────────────────┘

Note: Every change requires FULL RELINK (~30-90 sec depending on codebase size)
```

---

## Advantages

### 🎯 Single Binary

```
• One .exe to ship
• No DLL Hell
• Simpler deployment
• Easier debugging - full stack trace
```

### 🔌 Server Abstraction

```
• Clean API boundaries via servers
• RID-based opaque handles
• Easy to mock for tests
• Scene code never sees D3D12/Vulkan headers
```

### 🧩 Modular Compilation

```
# Build just physics module:
cmake --build . --target sparkle_physics

Only final link needs everything
```

### 📖 Godot-Proven

```
• Used by real shipping games
• MIT licensed
• Excellent documentation
• Large community
• Can learn from Godot's actual code
```

---

## Disadvantages

### 🐌 Link Time

Every change requires full relink:

```
Change 1 file in physics/
  → Compile 1 file (fast)
  → Link 50+ .a files (SLOW)
  → 60-120 seconds

Scales poorly as codebase grows
```

### 📏 Binary Size

Everything linked in:

```
sparkle.exe breakdown:
• Core:     2 MB
• Servers: 15 MB
• Scene:   20 MB
• Editor:  30 MB
─────────────────
Total:    67+ MB

Can't selectively load features
```

### 🔄 No Hot Reload

Can't reload code at runtime:

```
Every code change requires:
1. Stop game
2. Recompile
3. Relink (slow)
4. Restart game

Iteration loop: 2-3 minutes
```

### 🎮 Editor/Runtime Coupling

Same binary for editor and game:

```cpp
#ifdef TOOLS_ENABLED
    // Editor code
    void _edit_set_property(...) { }
#endif

// Lots of #ifdef throughout codebase
```

---

## When to Use This Architecture

✅ **Good for:**
- Solo developers or small teams (1-20)
- Simpler projects
- Learning engine architecture
- Maximum runtime performance
- Easy debugging

❌ **Avoid if:**
- Large team (50+ developers)
- Need fast iteration times
- Want hot-reload features
- Need minimal runtime binary

---

## Migration from Current State

**Timeline: 1-2 weeks**

```
Week 1:
├── Reorganize into core/, servers/, scene/, drivers/
├── Create Server interfaces
└── Move D3D12 code to drivers/

Week 2:
├── Implement RID system
├── Create scene tree structure
└── Test full build
```

---

## Example: Adding a New Feature

To add skeletal animation:

```
1. servers/animation/animation_server.h
   - Define abstract animation API
   
2. scene/animation/animation_player.h
   - Create node that uses AnimationServer
   
3. drivers/animation/animation_server_default.cpp
   - Implement the actual animation logic
   
4. modules/gltf/gltf_skeleton.cpp
   - Add glTF skeleton import
```

Scene code only sees server interface, never implementation details.
