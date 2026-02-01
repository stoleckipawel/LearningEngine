# Unity - DOTS/ECS Data Extraction Pattern

## Overview

Unity's Data-Oriented Technology Stack (DOTS) uses an **Entity Component System (ECS)** with a **Hybrid Renderer** that extracts render data from entities into cache-friendly arrays for GPU submission.

## Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                         ECS WORLD                                 │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │                      ENTITIES                                │ │
│  │  Entity 0: [LocalToWorld] [RenderMesh] [RenderBounds]       │ │
│  │  Entity 1: [LocalToWorld] [RenderMesh] [RenderBounds]       │ │
│  │  Entity 2: [LocalToWorld] [RenderMesh] [RenderBounds]       │ │
│  └─────────────────────────────────────────────────────────────┘ │
└───────────────────────────┬──────────────────────────────────────┘
                            │ BatchRendererGroup
                            ▼
┌──────────────────────────────────────────────────────────────────┐
│                    HYBRID RENDERER V2                             │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │               HeapBlocks (GPU Memory)                        │ │
│  │  ┌─────────────┬─────────────┬─────────────┬──────────────┐ │ │
│  │  │ Matrices[]  │ Colors[]    │ LightmapST[]│ Custom[]     │ │ │
│  │  │ [M0,M1,M2..│ [C0,C1,C2..│ [L0,L1,L2..│ [X0,X1,X2.. │ │ │
│  │  └─────────────┴─────────────┴─────────────┴──────────────┘ │ │
│  └─────────────────────────────────────────────────────────────┘ │
│                                                                   │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │                    BatchGroups                               │ │
│  │  BatchGroup[Mesh+Material Hash] → Draw commands              │ │
│  └─────────────────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Render Components (IComponentData)

```csharp
// Packages/com.unity.entities.graphics/Runtime/Components.cs

// Transform matrix for rendering
public struct LocalToWorld : IComponentData
{
    public float4x4 Value;
}

// Reference to the mesh to render
public struct RenderMesh : ISharedComponentData
{
    public Mesh mesh;
    public Material material;
    public int subMesh;
    public ShadowCastingMode castShadows;
    public bool receiveShadows;
}

// Bounds for culling
public struct RenderBounds : IComponentData
{
    public AABB Value;
}

// Per-instance material property overrides
public struct MaterialColor : IComponentData
{
    public float4 Value;
}

// LOD selection
public struct LODRange : IComponentData
{
    public float MinDistance;
    public float MaxDistance;
}
```

### 2. Batch Renderer Group

```csharp
// Unity's batching system for DOTS rendering
public class EntitiesGraphicsSystem : SystemBase
{
    private BatchRendererGroup m_BatchRendererGroup;
    private Dictionary<BatchKey, BatchInfo> m_Batches;
    
    protected override void OnCreate()
    {
        m_BatchRendererGroup = new BatchRendererGroup(
            OnPerformCulling,
            IntPtr.Zero);
    }
    
    // Called by Unity to cull and draw
    private JobHandle OnPerformCulling(
        BatchRendererGroup rendererGroup,
        BatchCullingContext cullingContext,
        BatchCullingOutput cullingOutput,
        IntPtr userContext)
    {
        // Schedule culling jobs
        var cullingJob = new CullBatchesJob
        {
            CullingPlanes = cullingContext.cullingPlanes,
            BatchBounds = m_BatchBounds,
            VisibleIndices = cullingOutput.visibleIndices
        };
        
        return cullingJob.Schedule(m_Batches.Count, 64);
    }
}
```

### 3. Data Extraction System

```csharp
// Extracts ECS data into GPU-friendly format
[UpdateInGroup(typeof(PresentationSystemGroup))]
public partial class UpdateMatricesSystem : SystemBase
{
    protected override void OnUpdate()
    {
        // Parallel job to extract matrices
        Entities
            .WithChangeFilter<LocalToWorld>()
            .ForEach((int entityInQueryIndex,
                      in LocalToWorld localToWorld,
                      in ChunkWorldRenderBounds bounds) =>
            {
                // Write directly to GPU buffer
                m_MatrixBuffer[entityInQueryIndex] = localToWorld.Value;
                m_BoundsBuffer[entityInQueryIndex] = bounds.Value;
            })
            .ScheduleParallel();
    }
}

// Change detection for efficient updates
[UpdateInGroup(typeof(PresentationSystemGroup))]
public partial class DetectChangedChunksSystem : SystemBase
{
    private EntityQuery m_ChangedQuery;
    
    protected override void OnCreate()
    {
        m_ChangedQuery = GetEntityQuery(
            ComponentType.ReadOnly<LocalToWorld>(),
            ComponentType.ReadOnly<ChunkWorldRenderBounds>());
        
        m_ChangedQuery.SetChangedVersionFilter(
            typeof(LocalToWorld));
    }
}
```

### 4. GPU Instancing Layout

```csharp
// How data is laid out for GPU instancing
public struct BatchDrawCommand
{
    public BatchMeshID meshID;
    public BatchMaterialID materialID;
    public uint flags;
    public int sortingPosition;
    public uint visibleOffset;
    public uint visibleCount;
}

// Metadata describing per-instance properties
public struct BatchMetadata
{
    public int instanceCount;
    public int matrixOffset;      // Offset into matrix buffer
    public int propertiesOffset;  // Offset into properties buffer
}

// GPU buffer layout
// Buffer 0: float4x4[] matrices (object to world)
// Buffer 1: float4x4[] prevMatrices (for motion vectors)
// Buffer 2: float4[] customProperties (colors, etc.)
```

## Data Flow

```
┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐
│  Game Systems   │   │ Transform System │   │ Render System   │
│                 │   │                  │   │                 │
│ Entity.Position │──►│ LocalToWorld     │──►│ GPU Matrix      │
│ Entity.Rotation │   │ Component        │   │ Buffer          │
│ Entity.Scale    │   │ Calculated       │   │                 │
└─────────────────┘   └─────────────────┘   └─────────────────┘
         │                    │                      │
         │                    │                      │
         ▼                    ▼                      ▼
    Archetype 0          Chunk Layout           Draw Commands
    [Pos][Rot][Scl]     [L2W][L2W][L2W]        [Mesh,Mat,Count]
```

## Chunk-Based Architecture

```csharp
// Entities are stored in chunks by archetype
// This enables cache-efficient iteration

// Archetype: entities with same component types
// Chunk: 16KB block storing multiple entities of same archetype

// Example chunk layout:
// |------- Chunk Header (64 bytes) -------|
// |------- LocalToWorld[64] --------------|
// |------- RenderBounds[64] ---------------|
// |------- Entity IDs[64] ----------------|

// Iteration is linear memory access
public void ProcessChunk(ArchetypeChunk chunk)
{
    var matrices = chunk.GetNativeArray(ref m_LocalToWorldHandle);
    var bounds = chunk.GetNativeArray(ref m_BoundsHandle);
    
    for (int i = 0; i < chunk.Count; i++)
    {
        // Cache-friendly sequential access
        ProcessEntity(matrices[i], bounds[i]);
    }
}
```

## Key Design Principles

1. **Data-Oriented Design**: Components are plain data, systems are pure functions
2. **Structural Sharing**: Entities with same components share chunk layout
3. **Change Detection**: Only process chunks that changed
4. **Burst Compilation**: C# compiled to native SIMD code
5. **Job System**: Automatic parallelization across cores

## Strengths

- Excellent cache utilization
- Massive parallelism (100k+ entities)
- Automatic GPU instancing
- Minimal per-frame allocations
- Change detection reduces work

## Weaknesses

- Steep learning curve
- Complex debugging
- Structural changes are expensive
- Limited OOP patterns
- Requires rethinking game architecture

## When to Use

- Large entity counts (thousands+)
- Performance-critical simulations
- Mobile games needing efficiency
- Procedural content with many instances
- Projects that can fully embrace ECS

## References

- [Unity DOTS Documentation](https://docs.unity3d.com/Packages/com.unity.entities@latest)
- [Entities Graphics Package](https://docs.unity3d.com/Packages/com.unity.entities.graphics@latest)
- GDC Talk: "A Data-Oriented Approach to Using Component Systems" - Unity
- [Unity DOTS Best Practices](https://unity.com/dots)
