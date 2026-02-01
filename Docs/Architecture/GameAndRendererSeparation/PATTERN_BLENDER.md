# Blender - Depsgraph + Draw Manager Pattern

## Overview

Blender uses a **Dependency Graph (Depsgraph)** to manage scene evaluation and a **Draw Manager** that abstracts the rendering backend. The game/render separation happens through evaluated copies of objects that the renderer consumes independently.

## Architecture

```
┌────────────────────────────────────────────────────────────────────┐
│                       ORIGINAL DATA (DNA)                           │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  bpy.data - All Blender data blocks                         │   │
│  │  - Objects (Object)                                          │   │
│  │  - Meshes (Mesh)                                             │   │
│  │  - Materials (Material)                                      │   │
│  │  - Armatures, Curves, etc.                                   │   │
│  │                                                              │   │
│  │  These are "original" datablocks - never modified by eval   │   │
│  └──────────────────────────┬──────────────────────────────────┘   │
└─────────────────────────────┼──────────────────────────────────────┘
                              │
                              │ Depsgraph Evaluation
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                      DEPENDENCY GRAPH                               │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Depsgraph                                 │   │
│  │                                                              │   │
│  │  ┌─────────────────────────────────────────────────────┐    │   │
│  │  │              ID Nodes (one per datablock)            │    │   │
│  │  │  ┌────────┐    ┌────────┐    ┌────────┐             │    │   │
│  │  │  │Object.A│───►│Mesh.A  │    │Material│             │    │   │
│  │  │  └───┬────┘    └────────┘    └────────┘             │    │   │
│  │  │      │                                               │    │   │
│  │  │      ▼                                               │    │   │
│  │  │  ┌────────┐                                          │    │   │
│  │  │  │Armature│ (dependency)                             │    │   │
│  │  │  └────────┘                                          │    │   │
│  │  └─────────────────────────────────────────────────────┘    │   │
│  │                                                              │   │
│  │  ┌─────────────────────────────────────────────────────┐    │   │
│  │  │           Evaluated Copies (COW)                     │    │   │
│  │  │  - Object_Eval (with final world matrix)             │    │   │
│  │  │  - Mesh_Eval (with modifiers applied)                │    │   │
│  │  │  - Material_Eval (with animated values)              │    │   │
│  │  └─────────────────────────────────────────────────────┘    │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
                              │
                              │ Draw Manager pulls evaluated data
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                       DRAW MANAGER (DRW)                            │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  DRWManager - Orchestrates all drawing                       │   │
│  │                                                              │   │
│  │  ┌──────────────────────┐  ┌──────────────────────┐         │   │
│  │  │   Draw Engines       │  │    DRW Data          │         │   │
│  │  │  - EEVEE             │  │  - DRWShadingGroup   │         │   │
│  │  │  - Workbench         │  │  - DRWPass          │         │   │
│  │  │  - Overlay           │  │  - DRWView          │         │   │
│  │  └──────────────────────┘  └──────────────────────┘         │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                         GPU MODULE                                  │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  GPU Abstraction Layer                                       │   │
│  │  - GPUBatch                                                  │   │
│  │  - GPUShader                                                 │   │
│  │  - GPUTexture                                                │   │
│  │  - GPUFrameBuffer                                            │   │
│  │  Backends: OpenGL, Vulkan, Metal                             │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Depsgraph (Dependency Graph)

```cpp
// source/blender/depsgraph/DEG_depsgraph.h

// Depsgraph is the evaluation context
struct Depsgraph {
    // Scene being evaluated
    Scene *scene;
    ViewLayer *view_layer;
    
    // All ID nodes in the graph
    Map<ID*, IDNode*> id_nodes;
    
    // Evaluation mode
    eEvaluationMode mode;  // VIEWPORT or RENDER
    
    // Frame being evaluated
    float ctime;
};

// Get evaluated version of an object
Object *DEG_get_evaluated_object(const Depsgraph *depsgraph, Object *ob);

// Get evaluated mesh (with modifiers applied)
Mesh *DEG_get_evaluated_mesh(const Depsgraph *depsgraph, Object *ob);

// Iterate over all evaluated objects
DEG_OBJECT_ITER_BEGIN(depsgraph, ob, DEG_ITER_OBJECT_FLAG_LINKED_DIRECTLY) {
    // ob is the evaluated copy
    draw_object(ob);
}
DEG_OBJECT_ITER_END;
```

### 2. Copy-On-Write (COW) System

```cpp
// source/blender/depsgraph/intern/eval/deg_eval_copy_on_write.cc

// Each ID can have an evaluated copy
struct IDNode {
    // Original datablock
    ID *id_orig;
    
    // Evaluated (COW) copy - created during evaluation
    ID *id_cow;
    
    // Evaluation function
    void (*eval_fn)(Depsgraph*, IDNode*);
    
    // Dependencies
    Vector<IDNode*> inlinks;   // Nodes this depends on
    Vector<IDNode*> outlinks;  // Nodes that depend on this
};

// Create COW copy
ID *deg_copy_on_write_datablock(const Depsgraph *depsgraph, const ID *id_orig)
{
    // Shallow copy of the datablock
    ID *id_cow = BKE_id_copy_ex(NULL, id_orig, NULL, LIB_ID_COPY_LOCALIZE);
    
    // Mark as evaluated
    id_cow->tag |= LIB_TAG_COPIED_ON_WRITE;
    
    return id_cow;
}
```

### 3. Object Evaluation

```cpp
// source/blender/depsgraph/intern/eval/deg_eval_transform.cc

void BKE_object_eval_transform_final(Depsgraph *depsgraph, Object *ob)
{
    // ob is the COW copy
    
    // Apply parent transform
    if (ob->parent) {
        Object *parent_eval = DEG_get_evaluated_object(depsgraph, ob->parent);
        mul_m4_m4m4(ob->obmat, parent_eval->obmat, ob->parentinv);
    }
    
    // Apply object's own transform
    BKE_object_to_mat4(ob, ob->obmat);
    
    // Apply constraints
    BKE_constraints_solve(depsgraph, &ob->constraints, ob, ctime);
    
    // Final world matrix is now in ob->obmat
}

void BKE_object_eval_mesh(Depsgraph *depsgraph, Object *ob)
{
    // Evaluate mesh with all modifiers
    Mesh *mesh_input = (Mesh *)ob->data;
    Mesh *mesh_eval = mesh_input;
    
    // Apply modifier stack
    for (ModifierData *md : ob->modifiers) {
        if (modifier_is_enabled(depsgraph, md)) {
            mesh_eval = modifier_apply(depsgraph, ob, md, mesh_eval);
        }
    }
    
    // Store evaluated mesh
    ob->runtime.mesh_eval = mesh_eval;
}
```

### 4. Draw Manager

```cpp
// source/blender/draw/DRW_engine.h

// Initialize drawing for a frame
void DRW_draw_view(const bContext *C);

// Internal draw manager state
typedef struct DRWManager {
    // Current depsgraph
    Depsgraph *depsgraph;
    
    // View/camera data
    DRWView *view;
    
    // Active draw engines
    Vector<DrawEngineType*> engines;
    
    // Resource pools
    DRWInstanceDataList *idatalist;
    DRWTextStore *text_store;
} DRWManager;

// Draw engine interface
typedef struct DrawEngineType {
    const char *name;
    
    // Called once at startup
    void (*engine_init)(void *vedata);
    
    // Called each frame - populate shading groups
    void (*cache_init)(void *vedata);
    void (*cache_populate)(void *vedata, Object *ob);
    void (*cache_finish)(void *vedata);
    
    // Actual drawing
    void (*draw_scene)(void *vedata);
    
} DrawEngineType;
```

### 5. EEVEE Draw Engine Example

```cpp
// source/blender/draw/engines/eevee/eevee_engine.c

static void eevee_cache_populate(void *vedata, Object *ob)
{
    EEVEE_Data *data = vedata;
    EEVEE_StorageList *stl = data->stl;
    
    // ob is already the evaluated copy from depsgraph
    
    if (ob->type == OB_MESH) {
        // Get evaluated mesh
        Mesh *mesh = BKE_object_get_evaluated_mesh(ob);
        
        // Create GPU batch for drawing
        GPUBatch *batch = DRW_mesh_batch_cache_get_surface(mesh);
        
        // Get material
        Material *ma = BKE_object_material_get_eval(ob, 0);
        
        // Add to appropriate shading group
        DRWShadingGroup *shgrp = eevee_material_get(data, ma);
        DRW_shgroup_call(shgrp, batch, ob->obmat);
    }
    else if (ob->type == OB_LAMP) {
        // Add light data
        EEVEE_lights_cache_add(stl, ob);
    }
}

static void eevee_draw_scene(void *vedata)
{
    EEVEE_Data *data = vedata;
    
    // GBuffer pass
    DRW_draw_pass(data->psl->gbuffer_ps);
    
    // Lighting pass
    EEVEE_lights_draw(data);
    
    // Screen-space effects
    EEVEE_effects_draw(data);
    
    // Composite final image
    DRW_draw_pass(data->psl->composite_ps);
}
```

### 6. DRW Shading Groups

```cpp
// source/blender/draw/intern/DRW_render.h

// A shading group is a collection of draw calls sharing same shader/state
typedef struct DRWShadingGroup {
    DRWShadingGroup *next;
    
    GPUShader *shader;
    
    // Uniform values
    DRWUniformChunk *uniforms;
    
    // Draw calls
    DRWCall *calls_first;
    DRWCall *calls_last;
    
    // State
    DRWState state;
} DRWShadingGroup;

// Add a draw call to a shading group
void DRW_shgroup_call(DRWShadingGroup *shgrp, GPUBatch *batch, float (*obmat)[4])
{
    DRWCall *call = BLI_mempool_alloc(DST.vmempool->calls);
    call->batch = batch;
    copy_m4_m4(call->obmat, obmat);
    
    // Link to shading group
    if (shgrp->calls_last) {
        shgrp->calls_last->next = call;
    } else {
        shgrp->calls_first = call;
    }
    shgrp->calls_last = call;
}
```

## Data Flow

```
User Edits Object              Depsgraph                    Draw Manager
       │                           │                            │
       │ Transform change          │                            │
       ├──────────────────────────►│                            │
       │                           │ Tag object dirty           │
       │                           │                            │
       │ Frame render requested    │                            │
       ├──────────────────────────►│                            │
       │                           │ Evaluate dirty nodes       │
       │                           │ Create/update COW copies   │
       │                           │                            │
       │                           │ DRW_draw_view()            │
       │                           ├───────────────────────────►│
       │                           │                            │
       │                           │ DEG_OBJECT_ITER            │
       │                           │◄───────────────────────────┤
       │                           │                            │
       │                           │ Evaluated objects          │
       │                           ├───────────────────────────►│
       │                           │                            │ cache_populate()
       │                           │                            │ Create batches
       │                           │                            │ Add to groups
       │                           │                            │
       │                           │                            │ draw_scene()
       │                           │                            │ Execute GPU
```

## Key Design Principles

1. **Copy-On-Write**: Evaluated data is separate from original
2. **Dependency Tracking**: Graph knows what needs re-evaluation
3. **Lazy Evaluation**: Only dirty nodes are re-evaluated
4. **Draw Engine Abstraction**: Multiple renderers share infrastructure
5. **Batched Drawing**: Shading groups minimize state changes

## Strengths

- Clean separation original/evaluated data
- Efficient incremental updates
- Multiple render engines supported
- Well-suited for DCC application
- Handles complex dependencies (modifiers, constraints)

## Weaknesses

- Complex system to understand
- Memory overhead for COW copies
- Not designed for real-time games
- Heavy abstraction layers

## When to Use

- DCC applications (editors, tools)
- Complex modifier/constraint systems
- Need for non-destructive workflows
- Multiple viewport/render modes
- Complex dependency relationships

## References

- [Blender Developer Documentation - Depsgraph](https://wiki.blender.org/wiki/Source/Depsgraph)
- [Blender Source - Draw Manager](https://developer.blender.org/docs/features/draw_manager/)
- [Blender Architecture Overview](https://wiki.blender.org/wiki/Source/Architecture)
- Blender Conference Talks on Depsgraph 2.0
