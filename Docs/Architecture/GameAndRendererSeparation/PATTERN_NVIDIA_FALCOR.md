# NVIDIA - Falcor Render Graph Pattern

## Overview

NVIDIA's Falcor framework uses a **Render Graph** system with strongly-typed **Render Passes** that communicate through named resource connections. This enables modular, reusable rendering techniques with automatic resource management.

## Architecture

```
┌────────────────────────────────────────────────────────────────────┐
│                         SCENE LAYER                                 │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Scene (CPU Data)                          │   │
│  │  - SceneBuilder creates scene from assets                    │   │
│  │  - Meshes, Materials, Lights, Cameras                        │   │
│  │  - Animation data                                            │   │
│  └──────────────────────────┬──────────────────────────────────┘   │
│                             │                                       │
│  ┌──────────────────────────▼──────────────────────────────────┐   │
│  │                   Scene (GPU Data)                           │   │
│  │  - Acceleration structures (BLAS/TLAS)                       │   │
│  │  - Material buffers                                          │   │
│  │  - Geometry buffers                                          │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                        RENDER GRAPH                                 │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  Named Connections Between Passes                            │   │
│  │                                                              │   │
│  │  ┌──────────────┐      ┌──────────────┐     ┌────────────┐  │   │
│  │  │  GBufferRT   │      │    SSAO      │     │  Lighting  │  │   │
│  │  │              │      │              │     │            │  │   │
│  │  │ out: depth ──┼─────►│ in: depth    │     │            │  │   │
│  │  │ out: normal ─┼─────►│ in: normals  │     │            │  │   │
│  │  │ out: albedo ─┼──────┼──────────────┼────►│ in: albedo │  │   │
│  │  │              │      │ out: aoMap ──┼────►│ in: ao     │  │   │
│  │  └──────────────┘      └──────────────┘     └────────────┘  │   │
│  │                                                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Scene Class

```cpp
// Falcor/Source/Falcor/Scene/Scene.h
class FALCOR_API Scene {
public:
    using SharedPtr = std::shared_ptr<Scene>;
    
    // Scene creation
    static SharedPtr create(const std::filesystem::path& path);
    static SharedPtr create(SceneBuilder& builder);
    
    // Access scene elements
    const std::vector<MeshInstance>& getMeshInstances() const { return mMeshInstances; }
    const std::vector<Light::SharedPtr>& getLights() const { return mLights; }
    const Camera::SharedPtr& getCamera() const { return mpCamera; }
    
    // GPU scene data
    struct GPUSceneData {
        Buffer::SharedPtr pMeshInstanceData;
        Buffer::SharedPtr pMaterialData;
        Buffer::SharedPtr pVertexBuffers[kMaxVertexBuffers];
        Buffer::SharedPtr pIndexBuffer;
        
        // Ray tracing structures
        std::vector<RtAccelerationStructure::SharedPtr> blasList;
        RtAccelerationStructure::SharedPtr pTlas;
    };
    
    const GPUSceneData& getGPUSceneData() const { return mGPUSceneData; }
    
    // Update for this frame
    void update(RenderContext* pRenderContext, double currentTime);
    
    // Bind scene to shader
    void setShaderData(const ShaderVar& var) const;
    
private:
    std::vector<MeshInstance> mMeshInstances;
    std::vector<Material::SharedPtr> mMaterials;
    std::vector<Light::SharedPtr> mLights;
    Camera::SharedPtr mpCamera;
    
    GPUSceneData mGPUSceneData;
    AnimationController::SharedPtr mpAnimationController;
};
```

### 2. RenderPass Base Class

```cpp
// Falcor/Source/Falcor/RenderGraph/RenderPass.h
class FALCOR_API RenderPass {
public:
    using SharedPtr = std::shared_ptr<RenderPass>;
    
    // Reflection info about pass inputs/outputs
    struct Reflection {
        struct Field {
            std::string name;
            ResourceFormat format;
            uint2 size;
            bool optional;
        };
        
        std::vector<Field> inputs;
        std::vector<Field> outputs;
    };
    
    // Implement in subclasses
    virtual Reflection reflect(const CompileData& compileData) = 0;
    
    // Called before execute to allocate resources
    virtual void compile(RenderContext* pRenderContext, 
                         const CompileData& compileData) {}
    
    // Execute the pass
    virtual void execute(RenderContext* pRenderContext, 
                         const RenderData& renderData) = 0;
    
    // UI for tweaking pass parameters
    virtual void renderUI(Gui::Widgets& widget) {}
    
    // Scene changed
    virtual void setScene(RenderContext* pRenderContext, 
                          const Scene::SharedPtr& pScene) {
        mpScene = pScene;
    }
    
protected:
    Scene::SharedPtr mpScene;
};
```

### 3. Example Render Pass: GBuffer

```cpp
// GBufferRT.h - Ray-traced GBuffer pass
class GBufferRT : public RenderPass {
public:
    static SharedPtr create(const Properties& props = {});
    
    // Declare inputs and outputs
    virtual Reflection reflect(const CompileData& compileData) override {
        Reflection r;
        
        // Outputs
        r.outputs.push_back({
            .name = "posW",
            .format = ResourceFormat::RGBA32Float,
            .size = compileData.defaultTexDims
        });
        r.outputs.push_back({
            .name = "normW",
            .format = ResourceFormat::RGBA32Float,
            .size = compileData.defaultTexDims
        });
        r.outputs.push_back({
            .name = "diffuseOpacity",
            .format = ResourceFormat::RGBA32Float,
            .size = compileData.defaultTexDims
        });
        r.outputs.push_back({
            .name = "depth",
            .format = ResourceFormat::R32Float,
            .size = compileData.defaultTexDims
        });
        
        return r;
    }
    
    virtual void execute(RenderContext* pRenderContext, 
                         const RenderData& renderData) override {
        // Get output textures from graph
        auto pPosW = renderData.getTexture("posW");
        auto pNormW = renderData.getTexture("normW");
        auto pDiffuse = renderData.getTexture("diffuseOpacity");
        auto pDepth = renderData.getTexture("depth");
        
        // Clear outputs
        pRenderContext->clearUAV(pPosW->getUAV().get(), float4(0));
        pRenderContext->clearUAV(pNormW->getUAV().get(), float4(0));
        
        // Bind scene
        auto var = mpRayTraceProgram->getRootVar();
        mpScene->setShaderData(var["gScene"]);
        
        // Bind outputs
        var["gPosW"] = pPosW;
        var["gNormW"] = pNormW;
        var["gDiffuseOpacity"] = pDiffuse;
        var["gDepth"] = pDepth;
        
        // Dispatch rays
        mpScene->raytrace(pRenderContext, mpRayTraceProgram.get(), mpVars,
            uint3(renderData.getDefaultTextureDims(), 1));
    }
    
private:
    RtProgram::SharedPtr mpRayTraceProgram;
    RtProgramVars::SharedPtr mpVars;
};
```

### 4. Render Graph Construction

```cpp
// Building a render graph
void buildRenderGraph(RenderGraph::SharedPtr pGraph, Scene::SharedPtr pScene) {
    
    // Add passes
    pGraph->addPass(GBufferRT::create(), "GBuffer");
    pGraph->addPass(SSAO::create(), "SSAO");
    pGraph->addPass(DeferredLighting::create(), "Lighting");
    pGraph->addPass(ToneMapper::create(), "ToneMap");
    pGraph->addPass(FXAA::create(), "FXAA");
    
    // Connect outputs to inputs by name
    pGraph->addEdge("GBuffer.posW", "SSAO.posW");
    pGraph->addEdge("GBuffer.normW", "SSAO.normW");
    pGraph->addEdge("GBuffer.depth", "SSAO.depth");
    
    pGraph->addEdge("GBuffer.diffuseOpacity", "Lighting.diffuse");
    pGraph->addEdge("GBuffer.normW", "Lighting.normal");
    pGraph->addEdge("GBuffer.posW", "Lighting.posW");
    pGraph->addEdge("SSAO.aoMap", "Lighting.ao");
    
    pGraph->addEdge("Lighting.color", "ToneMap.src");
    pGraph->addEdge("ToneMap.dst", "FXAA.src");
    
    // Mark final output
    pGraph->markOutput("FXAA.dst");
    
    // Set scene on all passes
    pGraph->setScene(pScene);
    
    // Compile - validates connections, allocates resources
    pGraph->compile();
}
```

### 5. RenderData (Pass Communication)

```cpp
// Data passed to each render pass
class RenderData {
public:
    // Get texture connected to this input/output
    Texture::SharedPtr getTexture(const std::string& name) const {
        auto it = mTextures.find(name);
        return (it != mTextures.end()) ? it->second : nullptr;
    }
    
    // Get buffer
    Buffer::SharedPtr getBuffer(const std::string& name) const;
    
    // Default dimensions for this frame
    uint2 getDefaultTextureDims() const { return mDefaultDims; }
    
    // Access to dictionary for custom pass data
    Properties& getProperties() { return mProperties; }
    
private:
    std::unordered_map<std::string, Texture::SharedPtr> mTextures;
    std::unordered_map<std::string, Buffer::SharedPtr> mBuffers;
    uint2 mDefaultDims;
    Properties mProperties;
};
```

## Scene Binding Pattern

```cpp
// Falcor's scene provides unified shader binding
// Shader side (HLSL):
struct Scene {
    StructuredBuffer<MeshInstance> meshInstances;
    StructuredBuffer<Material> materials;
    RaytracingAccelerationStructure tlas;
    
    // Helper functions
    MeshInstance getMeshInstance(uint index) {
        return meshInstances[index];
    }
    
    Material getMaterial(uint index) {
        return materials[index];
    }
};

// CPU side binding:
void MyPass::execute(RenderContext* ctx, const RenderData& renderData) {
    auto var = mpProgram->getRootVar();
    
    // One call binds entire scene
    mpScene->setShaderData(var["gScene"]);
    
    // Scene data is now available in shader:
    // - gScene.meshInstances
    // - gScene.materials  
    // - gScene.tlas
    // - gScene.lights
    // - gScene.camera
}
```

## Key Design Principles

1. **Named Connections**: Passes communicate via string-named resources
2. **Strong Typing**: Reflection system validates resource compatibility
3. **Scene Abstraction**: Scene encapsulates all renderable data
4. **Modular Passes**: Each pass is self-contained and reusable
5. **Automatic Resource Management**: Graph handles allocation/deallocation

## Strengths

- Very modular and reusable passes
- Easy to experiment with rendering techniques
- Strong type checking via reflection
- Excellent for research and prototyping
- Built-in ray tracing support

## Weaknesses

- String-based connections can be error-prone
- Runtime reflection overhead
- Designed for research, not shipping games
- Heavy abstraction layer

## When to Use

- Graphics research and prototyping
- Learning modern rendering techniques
- Need for ray tracing integration
- Rapid iteration on render features
- Academic or R&D projects

## References

- [Falcor GitHub Repository](https://github.com/NVIDIAGameWorks/Falcor)
- [Falcor Documentation](https://github.com/NVIDIAGameWorks/Falcor/tree/master/docs)
- NVIDIA GTC Talks on Falcor
- [NVIDIA Developer - Falcor](https://developer.nvidia.com/falcor)
