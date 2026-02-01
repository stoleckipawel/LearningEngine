# Godot Engine - RID Server Pattern

## Overview

Godot uses an **opaque handle (RID)** system where the game layer holds lightweight IDs, and all actual data lives in thread-safe **Server** singletons. This enables clean separation with minimal coupling.

## Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                       SCENE TREE (Game Layer)                     │
│                                                                   │
│  ┌─────────────┐   ┌─────────────┐   ┌─────────────────────────┐ │
│  │ MeshInstance│   │ Light3D     │   │ GPUParticles3D          │ │
│  │ mesh: RID   │   │ light: RID  │   │ particles: RID          │ │
│  │ instance:RID│   │ instance:RID│   │ instance: RID           │ │
│  └──────┬──────┘   └──────┬──────┘   └───────────┬─────────────┘ │
│         │                 │                      │               │
└─────────┼─────────────────┼──────────────────────┼───────────────┘
          │                 │                      │
          │    RID = opaque 64-bit handle          │
          ▼                 ▼                      ▼
┌──────────────────────────────────────────────────────────────────┐
│                      RENDERING SERVER                             │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │  RID_Owner<Mesh>        RID_Owner<Light>   RID_Owner<...>  │  │
│  │  ┌─────────────────┐   ┌─────────────────┐                 │  │
│  │  │ [RID→MeshData]  │   │ [RID→LightData] │                 │  │
│  │  │ surfaces[]      │   │ color, range    │                 │  │
│  │  │ aabb            │   │ shadow_enabled  │                 │  │
│  │  └─────────────────┘   └─────────────────┘                 │  │
│  └────────────────────────────────────────────────────────────┘  │
│                                                                   │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │                    Rendering Pipeline                       │  │
│  │  - Visibility culling                                       │  │
│  │  - Shadow mapping                                           │  │
│  │  - Forward/Deferred rendering                              │  │
│  └────────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. RID (Resource ID)

```cpp
// core/templates/rid.h
class RID {
    uint64_t _id = 0;
    
public:
    _FORCE_INLINE_ bool is_valid() const { return _id != 0; }
    _FORCE_INLINE_ bool is_null() const { return _id == 0; }
    _FORCE_INLINE_ uint64_t get_id() const { return _id; }
    
    // Comparison operators
    _FORCE_INLINE_ bool operator==(const RID &p_rid) const {
        return _id == p_rid._id;
    }
    _FORCE_INLINE_ bool operator<(const RID &p_rid) const {
        return _id < p_rid._id;
    }
};
```

### 2. RID_Owner (Type-Safe Handle Storage)

```cpp
// core/templates/rid_owner.h
template <typename T>
class RID_Owner {
    // Slot-based storage with free list
    struct Chunk {
        T data[CHUNK_SIZE];
        uint32_t validator[CHUNK_SIZE];
    };
    
    Vector<Chunk*> chunks;
    uint32_t free_list_head = UINT32_MAX;
    
public:
    // Create new resource, return handle
    RID make_rid(const T &p_data) {
        uint32_t slot = allocate_slot();
        chunks[slot / CHUNK_SIZE]->data[slot % CHUNK_SIZE] = p_data;
        
        RID rid;
        rid._id = (uint64_t(validator) << 32) | slot;
        return rid;
    }
    
    // Get resource from handle (with validation)
    T* get_or_null(RID p_rid) {
        uint32_t slot = p_rid._id & 0xFFFFFFFF;
        uint32_t validator = p_rid._id >> 32;
        
        if (chunks[slot / CHUNK_SIZE]->validator[slot % CHUNK_SIZE] != validator) {
            return nullptr; // Invalid or stale handle
        }
        return &chunks[slot / CHUNK_SIZE]->data[slot % CHUNK_SIZE];
    }
    
    // Free resource
    void free(RID p_rid) {
        uint32_t slot = p_rid._id & 0xFFFFFFFF;
        chunks[slot / CHUNK_SIZE]->validator[slot % CHUNK_SIZE]++;
        // Add to free list
        return_to_free_list(slot);
    }
};
```

### 3. RenderingServer API

```cpp
// servers/rendering_server.h
class RenderingServer : public Object {
    GDCLASS(RenderingServer, Object);
    
    static RenderingServer *singleton;
    
public:
    static RenderingServer *get_singleton() { return singleton; }
    
    // ==================== MESH ====================
    virtual RID mesh_create() = 0;
    virtual void mesh_add_surface(RID p_mesh, const SurfaceData &p_surface) = 0;
    virtual void mesh_set_custom_aabb(RID p_mesh, const AABB &p_aabb) = 0;
    virtual AABB mesh_get_aabb(RID p_mesh) const = 0;
    virtual void mesh_free(RID p_mesh) = 0;
    
    // ==================== TEXTURE ====================
    virtual RID texture_create() = 0;
    virtual void texture_set_data(RID p_texture, const Ref<Image> &p_image) = 0;
    virtual Ref<Image> texture_get_data(RID p_texture) const = 0;
    
    // ==================== MATERIAL ====================
    virtual RID material_create() = 0;
    virtual void material_set_shader(RID p_material, RID p_shader) = 0;
    virtual void material_set_param(RID p_material, const StringName &p_param, const Variant &p_value) = 0;
    
    // ==================== INSTANCE ====================
    // Instances are what gets rendered (combines mesh + material + transform)
    virtual RID instance_create() = 0;
    virtual void instance_set_base(RID p_instance, RID p_base) = 0;
    virtual void instance_set_transform(RID p_instance, const Transform3D &p_transform) = 0;
    virtual void instance_set_scenario(RID p_instance, RID p_scenario) = 0;
    virtual void instance_set_visible(RID p_instance, bool p_visible) = 0;
    
    // ==================== SCENARIO (Scene) ====================
    virtual RID scenario_create() = 0;
    virtual void scenario_set_environment(RID p_scenario, RID p_environment) = 0;
    
    // ==================== VIEWPORT ====================
    virtual RID viewport_create() = 0;
    virtual void viewport_set_scenario(RID p_viewport, RID p_scenario) = 0;
    virtual void viewport_set_size(RID p_viewport, int p_width, int p_height) = 0;
};
```

### 4. MeshInstance3D (Scene Node)

```cpp
// scene/3d/mesh_instance_3d.h
class MeshInstance3D : public GeometryInstance3D {
    GDCLASS(MeshInstance3D, GeometryInstance3D);
    
    Ref<Mesh> mesh;  // Resource (contains RID internally)
    RID instance;     // Rendering instance handle
    
protected:
    void _notification(int p_what) {
        switch (p_what) {
            case NOTIFICATION_ENTER_TREE: {
                // Create rendering instance
                instance = RS::get_singleton()->instance_create();
                RS::get_singleton()->instance_set_scenario(
                    instance, get_world_3d()->get_scenario());
                    
                if (mesh.is_valid()) {
                    RS::get_singleton()->instance_set_base(
                        instance, mesh->get_rid());
                }
            } break;
            
            case NOTIFICATION_TRANSFORM_CHANGED: {
                // Update transform in renderer
                RS::get_singleton()->instance_set_transform(
                    instance, get_global_transform());
            } break;
            
            case NOTIFICATION_EXIT_TREE: {
                // Cleanup
                RS::get_singleton()->instance_free(instance);
                instance = RID();
            } break;
        }
    }
    
public:
    void set_mesh(const Ref<Mesh> &p_mesh) {
        mesh = p_mesh;
        if (instance.is_valid() && mesh.is_valid()) {
            RS::get_singleton()->instance_set_base(
                instance, mesh->get_rid());
        }
    }
};
```

## Data Flow

```
Scene Tree                    RenderingServer
    │                              │
    │  mesh_create()               │
    │◄─────────────────────────────┤ Returns RID
    │                              │
    │  instance_create()           │
    │◄─────────────────────────────┤ Returns RID  
    │                              │
    │  instance_set_base(inst,mesh)│
    ├─────────────────────────────►│
    │                              │ Links instance to mesh
    │                              │
    │  instance_set_transform()    │
    ├─────────────────────────────►│
    │                              │ Updates transform
    │                              │
    │          Render Frame        │
    │                              │
    │                              │ Culling
    │                              │ Sorting
    │                              │ Drawing
```

## Thread Safety

```cpp
// Command queue for thread-safe updates
class RenderingServerDefault : public RenderingServer {
    // Commands queued from main thread
    CommandQueueMT command_queue;
    
    // Executed on render thread
    void _thread_loop() {
        while (running) {
            command_queue.flush_all();
            _render_frame();
        }
    }
    
public:
    virtual void instance_set_transform(RID p_instance, 
                                        const Transform3D &p_transform) override {
        command_queue.push(this, 
            &RenderingServerDefault::_instance_set_transform_impl,
            p_instance, p_transform);
    }
};
```

## Key Design Principles

1. **Opaque Handles**: Game code never sees render data structures
2. **Server Singletons**: Centralized, thread-safe access
3. **Resource Ownership**: Server owns all render resources
4. **Validation**: RIDs include generation counter to detect stale handles
5. **Command Queue**: Thread-safe batching of render commands

## Strengths

- Clean API boundary
- Minimal game/render coupling  
- Easy serialization (RIDs are just numbers)
- Thread-safe by design
- Resource leak detection

## Weaknesses

- Indirection overhead on every access
- Harder to batch related operations
- Debug inspection requires server queries
- Handle management complexity

## When to Use

- Clean modular architecture priority
- Multiple rendering backends
- Need for thread safety
- Game/editor separation
- Scripting language bindings

## References

- [Godot RenderingServer Documentation](https://docs.godotengine.org/en/stable/classes/class_renderingserver.html)
- [Godot Source - RID Owner](https://github.com/godotengine/godot/blob/master/core/templates/rid_owner.h)
- [Godot Architecture Overview](https://docs.godotengine.org/en/stable/contributing/development/core_and_modules/index.html)
