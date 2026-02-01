# AMD - GPUOpen Frame Graph Pattern

## Overview

AMD's GPUOpen initiative promotes a **Frame Graph** architecture that decouples the game simulation from rendering by describing the entire frame as a directed acyclic graph (DAG) of render passes. This enables automatic resource management, barrier optimization, and parallel execution.

## Architecture

```
┌────────────────────────────────────────────────────────────────────┐
│                          GAME LAYER                                 │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Scene Graph                               │   │
│  │  - Entity transforms, visibility                             │   │
│  │  - Mesh/Material references                                  │   │
│  │  - Light parameters                                          │   │
│  └──────────────────────────┬──────────────────────────────────┘   │
└─────────────────────────────┼──────────────────────────────────────┘
                              │ Extract visible objects
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                       FRAME GRAPH BUILDER                           │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  RenderGraph::Build()                                        │   │
│  │  ├── AddPass("GBuffer")                                      │   │
│  │  │     Creates: GBufferAlbedo, GBufferNormal, DepthBuffer   │   │
│  │  ├── AddPass("ShadowMap")                                    │   │
│  │  │     Creates: ShadowAtlas                                  │   │
│  │  ├── AddPass("Lighting")                                     │   │
│  │  │     Reads: GBuffer*, ShadowAtlas                         │   │
│  │  │     Creates: HDRColor                                     │   │
│  │  ├── AddPass("PostProcess")                                  │   │
│  │  │     Reads: HDRColor                                       │   │
│  │  │     Creates: FinalOutput                                  │   │
│  │  └── Compile() → Execution order + barriers                  │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                    COMPILED FRAME GRAPH                             │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  Pass Execution Order (topologically sorted)                 │   │
│  │  ┌──────────┬──────────┬──────────┬───────────────┐         │   │
│  │  │ ShadowMap│ GBuffer  │ Lighting │ PostProcess   │         │   │
│  │  └────┬─────┴────┬─────┴────┬─────┴───────┬───────┘         │   │
│  │       │          │          │             │                  │   │
│  │  Barriers    Barriers   Barriers     Barriers               │   │
│  │  auto-       auto-      auto-        auto-                  │   │
│  │  inserted    inserted   inserted     inserted               │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Render Graph Builder

```cpp
// Based on AMD FidelityFX / Cauldron patterns
class RenderGraph {
public:
    // Transient resource handle (virtual until compiled)
    using ResourceHandle = uint32_t;
    
    // Declare a render pass
    template<typename PassData>
    PassData& AddPass(const char* name, 
                      RenderPassType type,
                      std::function<void(RenderGraphBuilder&, PassData&)> setup,
                      std::function<void(const PassData&, RenderContext&)> execute)
    {
        PassData* data = new PassData();
        
        RenderGraphBuilder builder(*this, name);
        setup(builder, *data);
        
        m_Passes.push_back({
            .name = name,
            .type = type,
            .data = data,
            .execute = [=](RenderContext& ctx) { execute(*data, ctx); }
        });
        
        return *data;
    }
    
    // Compile: determine execution order, allocate resources
    void Compile() {
        // 1. Build dependency graph from resource reads/writes
        BuildDependencyGraph();
        
        // 2. Topological sort to get execution order
        TopologicalSort();
        
        // 3. Calculate resource lifetimes
        CalculateResourceLifetimes();
        
        // 4. Allocate physical resources with aliasing
        AllocateResources();
        
        // 5. Insert barriers between passes
        InsertBarriers();
    }
    
    // Execute all passes
    void Execute(RenderContext& context) {
        for (auto& pass : m_ExecutionOrder) {
            context.BeginPass(pass.name);
            
            // Apply barriers
            for (auto& barrier : pass.barriers) {
                context.ResourceBarrier(barrier);
            }
            
            // Execute pass
            pass.execute(context);
            
            context.EndPass();
        }
    }
    
private:
    std::vector<PassInfo> m_Passes;
    std::vector<PassInfo*> m_ExecutionOrder;
    ResourceAllocator m_Allocator;
};
```

### 2. Render Graph Builder (Per-Pass)

```cpp
class RenderGraphBuilder {
public:
    // Declare that this pass CREATES a resource
    ResourceHandle CreateTexture(const TextureDesc& desc) {
        return m_Graph.AllocateVirtualResource(desc);
    }
    
    // Declare that this pass READS a resource
    ResourceHandle Read(ResourceHandle resource, ResourceState state) {
        m_CurrentPass.reads.push_back({resource, state});
        return resource;
    }
    
    // Declare that this pass WRITES to a resource
    ResourceHandle Write(ResourceHandle resource, ResourceState state) {
        m_CurrentPass.writes.push_back({resource, state});
        return resource;
    }
    
    // Read/Write (UAV access)
    ResourceHandle ReadWrite(ResourceHandle resource) {
        Read(resource, ResourceState::UnorderedAccess);
        Write(resource, ResourceState::UnorderedAccess);
        return resource;
    }
};
```

### 3. Example Frame Setup

```cpp
void BuildFrame(RenderGraph& graph, const SceneView& view) {
    
    // ============ SHADOW PASS ============
    struct ShadowPassData {
        ResourceHandle shadowAtlas;
        std::vector<ShadowCaster> casters;
    };
    
    auto& shadowData = graph.AddPass<ShadowPassData>(
        "ShadowMap",
        RenderPassType::Graphics,
        // Setup lambda
        [&](RenderGraphBuilder& builder, ShadowPassData& data) {
            data.shadowAtlas = builder.CreateTexture({
                .width = 4096, .height = 4096,
                .format = Format::D32_FLOAT,
                .usage = TextureUsage::DepthStencil | TextureUsage::ShaderResource
            });
            
            // Extract shadow casters from scene
            for (auto& light : view.lights) {
                if (light.castsShadow) {
                    data.casters.push_back(ExtractShadowCaster(light, view));
                }
            }
        },
        // Execute lambda  
        [](const ShadowPassData& data, RenderContext& ctx) {
            for (auto& caster : data.casters) {
                ctx.SetRenderTarget(nullptr, data.shadowAtlas);
                ctx.SetViewport(caster.viewport);
                RenderShadowCaster(ctx, caster);
            }
        }
    );
    
    // ============ GBUFFER PASS ============
    struct GBufferPassData {
        ResourceHandle albedo;
        ResourceHandle normal;
        ResourceHandle depth;
        const SceneView* view;
    };
    
    auto& gbufferData = graph.AddPass<GBufferPassData>(
        "GBuffer",
        RenderPassType::Graphics,
        [&](RenderGraphBuilder& builder, GBufferPassData& data) {
            data.albedo = builder.CreateTexture({
                .width = view.width, .height = view.height,
                .format = Format::RGBA8_SRGB
            });
            data.normal = builder.CreateTexture({
                .width = view.width, .height = view.height,
                .format = Format::RGB10A2_UNORM
            });
            data.depth = builder.CreateTexture({
                .width = view.width, .height = view.height,
                .format = Format::D32_FLOAT
            });
            data.view = &view;
        },
        [](const GBufferPassData& data, RenderContext& ctx) {
            ctx.SetRenderTargets({data.albedo, data.normal}, data.depth);
            RenderGBuffer(ctx, *data.view);
        }
    );
    
    // ============ LIGHTING PASS ============
    struct LightingPassData {
        ResourceHandle albedo;
        ResourceHandle normal;
        ResourceHandle depth;
        ResourceHandle shadowAtlas;
        ResourceHandle hdrOutput;
    };
    
    graph.AddPass<LightingPassData>(
        "Lighting",
        RenderPassType::Compute,
        [&](RenderGraphBuilder& builder, LightingPassData& data) {
            // Read GBuffer outputs
            data.albedo = builder.Read(gbufferData.albedo, ResourceState::ShaderResource);
            data.normal = builder.Read(gbufferData.normal, ResourceState::ShaderResource);
            data.depth = builder.Read(gbufferData.depth, ResourceState::ShaderResource);
            data.shadowAtlas = builder.Read(shadowData.shadowAtlas, ResourceState::ShaderResource);
            
            // Create HDR output
            data.hdrOutput = builder.CreateTexture({
                .width = view.width, .height = view.height,
                .format = Format::RGBA16_FLOAT
            });
        },
        [](const LightingPassData& data, RenderContext& ctx) {
            ctx.BindTexture(0, data.albedo);
            ctx.BindTexture(1, data.normal);
            ctx.BindTexture(2, data.depth);
            ctx.BindTexture(3, data.shadowAtlas);
            ctx.BindUAV(0, data.hdrOutput);
            ctx.Dispatch(DivCeil(width, 8), DivCeil(height, 8), 1);
        }
    );
}
```

### 4. Resource Aliasing

```cpp
// Automatic memory aliasing for transient resources
class ResourceAllocator {
    struct MemoryPool {
        uint64_t size;
        std::vector<AllocationRange> freeRanges;
    };
    
public:
    void AllocateResources(RenderGraph& graph) {
        // Sort resources by lifetime end (earliest first)
        std::vector<VirtualResource*> resources = graph.GetResources();
        std::sort(resources.begin(), resources.end(),
            [](auto* a, auto* b) { return a->lifetimeEnd < b->lifetimeEnd; });
        
        for (auto* resource : resources) {
            // Try to alias with a dead resource
            PhysicalResource* physical = FindAliasCandidate(
                resource->desc,
                resource->lifetimeStart
            );
            
            if (!physical) {
                // No alias possible, allocate new
                physical = AllocatePhysical(resource->desc);
            }
            
            resource->physical = physical;
            physical->lifetimeEnd = resource->lifetimeEnd;
        }
    }
};
```

## Key Design Principles

1. **Declarative Rendering**: Describe WHAT to render, not HOW
2. **Automatic Barriers**: Graph compiler inserts optimal synchronization
3. **Resource Aliasing**: Transient resources share memory based on lifetime
4. **Deferred Execution**: All passes declared before any execution
5. **Data Extraction**: Pass setup captures scene data

## Strengths

- Optimal barrier placement
- Memory efficiency through aliasing
- Easy to add/remove render features
- Clear visualization of frame structure
- Parallel pass execution potential

## Weaknesses

- Compilation overhead each frame
- Complex debugging when things go wrong
- Pass ordering constraints can be subtle
- Learning curve for graph-based thinking

## When to Use

- Modern graphics APIs (DX12, Vulkan, Metal)
- Complex multi-pass rendering
- Performance-critical AAA rendering
- Need for automatic resource management
- Async compute utilization

## References

- [AMD GPUOpen - Render Graph](https://gpuopen.com/learn/render-graphs/)
- [FidelityFX SDK Architecture](https://github.com/GPUOpen-Effects/FidelityFX-SDK)
- [Cauldron Framework](https://github.com/GPUOpen-LibrariesAndSDKs/Cauldron)
- GDC 2017: "FrameGraph: Extensible Rendering Architecture in Frostbite"
