# Epic Games - Unreal Engine Render Proxy Pattern

## Overview

Unreal Engine uses a **Render Proxy** pattern where game-thread components create corresponding render-thread proxies. This provides complete thread isolation with explicit ownership semantics.

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        GAME THREAD                               │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐  │
│  │ UActorComponent│   │UPrimitiveComponent│  │UStaticMeshComponent│ │
│  └──────┬──────┘    └──────┬──────┘    └──────────┬──────────┘  │
│         │                  │                       │             │
│         │    CreateSceneProxy()                    │             │
│         ▼                  ▼                       ▼             │
├─────────────────────────────────────────────────────────────────┤
│              RENDER COMMAND QUEUE (FIFO)                         │
├─────────────────────────────────────────────────────────────────┤
│                       RENDER THREAD                              │
│  ┌─────────────┐    ┌─────────────────┐    ┌─────────────────┐  │
│  │FSceneProxy  │    │FPrimitiveSceneProxy│ │FStaticMeshSceneProxy││
│  └──────┬──────┘    └──────┬──────────┘    └──────┬──────────┘  │
│         │                  │                       │             │
│         ▼                  ▼                       ▼             │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                      FScene                                  ││
│  │  - Octree spatial structure                                  ││
│  │  - Primitive visibility                                      ││
│  │  - Light interactions                                        ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. UPrimitiveComponent (Game Thread)

```cpp
// Engine/Source/Runtime/Engine/Classes/Components/PrimitiveComponent.h
class UPrimitiveComponent : public USceneComponent
{
public:
    // Creates the render thread representation
    virtual FPrimitiveSceneProxy* CreateSceneProxy()
    {
        return nullptr; // Override in subclasses
    }
    
    // Called when transform changes
    void SendRenderTransform_Concurrent();
    
    // Mark render state dirty - will recreate proxy
    void MarkRenderStateDirty();
    
    // Dynamic data update (doesn't recreate proxy)
    void MarkRenderDynamicDataDirty();
    
protected:
    // Cached render-thread ID
    FPrimitiveComponentId ComponentId;
    
    // Bounds for culling
    FBoxSphereBounds Bounds;
};
```

### 2. FPrimitiveSceneProxy (Render Thread)

```cpp
// Engine/Source/Runtime/Engine/Public/PrimitiveSceneProxy.h
class FPrimitiveSceneProxy
{
public:
    FPrimitiveSceneProxy(const UPrimitiveComponent* InComponent)
        : PrimitiveComponentId(InComponent->ComponentId)
        , bCastShadow(InComponent->CastShadow)
        , Mobility(InComponent->Mobility)
        , LocalBounds(InComponent->Bounds)
    {
        // Copy ALL needed data from component
        // After constructor, component pointer is INVALID
    }
    
    virtual ~FPrimitiveSceneProxy() {}
    
    // Called every frame for visible primitives
    virtual void GetDynamicMeshElements(
        const TArray<const FSceneView*>& Views,
        const FSceneViewFamily& ViewFamily,
        uint32 VisibilityMap,
        FMeshElementCollector& Collector) const = 0;
    
    // Static mesh path - returns cached mesh batches
    virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) {}
    
    // Bounds for visibility culling
    virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const;
    
protected:
    // Immutable data copied from component
    const FPrimitiveComponentId PrimitiveComponentId;
    const bool bCastShadow;
    const EComponentMobility::Type Mobility;
    const FBoxSphereBounds LocalBounds;
    
    // Transform updated via render commands
    FMatrix LocalToWorld;
};
```

### 3. FScene (Render Thread Scene)

```cpp
// Engine/Source/Runtime/Renderer/Private/ScenePrivate.h
class FScene : public FSceneInterface
{
public:
    // Add primitive to scene
    virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
    
    // Remove primitive from scene
    virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;
    
    // Update primitive transform
    virtual void UpdatePrimitiveTransform(UPrimitiveComponent* Primitive) override;
    
private:
    // Spatial acceleration structure
    FSceneOctree PrimitiveOctree;
    
    // All primitives in scene
    TArray<FPrimitiveSceneInfo*> Primitives;
    
    // Lights
    TArray<FLightSceneInfo*> Lights;
    
    // Added primitives pending processing
    TArray<FPrimitiveSceneInfo*> AddedPrimitiveSceneInfos;
};
```

### 4. Render Commands

```cpp
// Thread-safe command enqueue
ENQUEUE_RENDER_COMMAND(AddPrimitiveCommand)(
    [Scene, PrimitiveSceneInfo](FRHICommandListImmediate& RHICmdList)
    {
        Scene->AddPrimitiveSceneInfo_RenderThread(PrimitiveSceneInfo);
    });

// Update transform example
void UPrimitiveComponent::SendRenderTransform_Concurrent()
{
    FMatrix LocalToWorld = GetComponentTransform().ToMatrixWithScale();
    FPrimitiveComponentId Id = ComponentId;
    
    ENQUEUE_RENDER_COMMAND(UpdateTransformCommand)(
        [Id, LocalToWorld](FRHICommandListImmediate& RHICmdList)
        {
            FPrimitiveSceneInfo* Info = GetScene()->GetPrimitiveInfo(Id);
            if (Info && Info->Proxy)
            {
                Info->Proxy->SetTransform(LocalToWorld);
            }
        });
}
```

## Data Flow

```
Game Thread                    Render Thread
    │                              │
    │  CreateSceneProxy()          │
    ├─────────────────────────────►│
    │                              │ FPrimitiveSceneProxy created
    │                              │ Added to FScene
    │                              │
    │  Transform Update            │
    ├─────────────────────────────►│
    │                              │ Proxy transform updated
    │                              │
    │  MarkRenderStateDirty()      │
    ├─────────────────────────────►│
    │                              │ Old proxy destroyed
    │                              │ New proxy created
    │                              │
    │  DestroyComponent()          │
    ├─────────────────────────────►│
    │                              │ Proxy removed from scene
    │                              │ Proxy destroyed
```

## Key Design Principles

1. **Complete Data Copy**: Proxy copies ALL needed data at creation
2. **One-Way Communication**: Game→Render only via commands
3. **Explicit Ownership**: Proxy lifetime managed by render thread
4. **Spatial Coherence**: FScene uses octree for visibility
5. **Material Proxies**: Similar pattern for UMaterialInterface → FMaterial

## Strengths

- Complete thread isolation
- Predictable performance
- Clear ownership semantics
- Supports complex materials and mesh LOD
- Battle-tested at massive scale (Fortnite, etc.)

## Weaknesses

- High memory overhead (duplicate data)
- Complex proxy recreation logic
- Latency from command queue
- Difficult to debug cross-thread issues

## When to Use

- Large AAA games with complex rendering
- Projects needing parallel game/render threads
- Heavy use of complex materials and lighting
- Multi-platform console/PC titles

## References

- [Unreal Engine Source - PrimitiveComponent.h](https://github.com/EpicGames/UnrealEngine)
- [Unreal Rendering Documentation](https://docs.unrealengine.com/5.0/en-US/graphics-programming-overview-for-unreal-engine/)
- GDC Talk: "Parallelizing the Naughty Dog Engine Using Fibers" (similar concepts)
