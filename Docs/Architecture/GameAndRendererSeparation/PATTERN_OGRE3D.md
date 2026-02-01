# OGRE 3D - Scene Manager Pattern

## Overview

OGRE (Object-Oriented Graphics Rendering Engine) uses a classic **Scene Manager** pattern with **MovableObjects** attached to **SceneNodes**. The separation is achieved through a clear hierarchy of scene management that abstracts rendering from game logic.

## Architecture

```
┌────────────────────────────────────────────────────────────────────┐
│                          GAME LAYER                                 │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                 Application / Game Code                      │   │
│  │  - Creates SceneNodes                                        │   │
│  │  - Attaches MovableObjects (Entity, Light, Camera)          │   │
│  │  - Sets positions, rotations via SceneNode                  │   │
│  └──────────────────────────┬──────────────────────────────────┘   │
└─────────────────────────────┼──────────────────────────────────────┘
                              │
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                       SCENE MANAGER                                 │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    SceneManager                              │   │
│  │  ┌─────────────────────────────────────────────────────┐    │   │
│  │  │                    SceneNode Tree                    │    │   │
│  │  │        ┌─────────────┐                              │    │   │
│  │  │        │  RootNode   │                              │    │   │
│  │  │        └──────┬──────┘                              │    │   │
│  │  │     ┌─────────┼─────────┐                           │    │   │
│  │  │     ▼         ▼         ▼                           │    │   │
│  │  │ ┌───────┐ ┌───────┐ ┌───────┐                       │    │   │
│  │  │ │Node A │ │Node B │ │Node C │                       │    │   │
│  │  │ │Entity │ │Light  │ │Camera │                       │    │   │
│  │  │ └───────┘ └───────┘ └───────┘                       │    │   │
│  │  └─────────────────────────────────────────────────────┘    │   │
│  │                                                              │   │
│  │  ┌─────────────────────────────────────────────────────┐    │   │
│  │  │              MovableObject Registry                  │    │   │
│  │  │  - Entities (meshes)                                 │    │   │
│  │  │  - Lights                                            │    │   │
│  │  │  - Cameras                                           │    │   │
│  │  │  - ParticleSystems                                   │    │   │
│  │  │  - BillboardSets                                     │    │   │
│  │  └─────────────────────────────────────────────────────┘    │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                      │
│                              │ _renderScene()                       │
│                              ▼                                      │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                   RenderQueue                                │   │
│  │  - Sorted by material                                        │   │
│  │  - Sorted by distance                                        │   │
│  │  - Grouped by render group                                   │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌────────────────────────────────────────────────────────────────────┐
│                       RENDER SYSTEM                                 │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  RenderSystem (D3D11, GL3+, Metal, Vulkan)                  │   │
│  │  - State management                                          │   │
│  │  - Draw call execution                                       │   │
│  │  - Resource management                                       │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. SceneNode

```cpp
// OgreSceneNode.h
class _OgreExport SceneNode : public Node {
public:
    // Create child node
    SceneNode* createChildSceneNode(
        const Vector3& translate = Vector3::ZERO,
        const Quaternion& rotate = Quaternion::IDENTITY);
    
    // Attach renderable objects
    void attachObject(MovableObject* obj);
    void detachObject(MovableObject* obj);
    void detachAllObjects();
    
    // Get attached objects
    ObjectIterator getAttachedObjectIterator();
    MovableObject* getAttachedObject(const String& name);
    
    // Visibility
    void setVisible(bool visible, bool cascade = true);
    
    // Get world transform (accumulated from parents)
    const Matrix4& _getFullTransform() const;
    
    // Find visible objects for rendering
    void _findVisibleObjects(Camera* cam, RenderQueue* queue,
                             VisibleObjectsBoundsInfo* visibleBounds,
                             bool includeChildren = true,
                             bool displayNodes = false);
    
protected:
    // Attached MovableObjects
    ObjectMap mObjectsByName;
    
    // Cached world transform
    mutable Matrix4 mCachedTransform;
    mutable bool mCachedTransformOutOfDate;
};
```

### 2. MovableObject (Base for all renderables)

```cpp
// OgreMovableObject.h
class _OgreExport MovableObject : public AnimableObject {
public:
    // Unique name
    const String& getName() const { return mName; }
    
    // Parent scene node
    SceneNode* getParentSceneNode() const { return mParentNode; }
    
    // Bounding volume
    virtual const AxisAlignedBox& getBoundingBox() const = 0;
    virtual Real getBoundingRadius() const = 0;
    
    // World bounding box (transformed)
    const AxisAlignedBox& getWorldBoundingBox(bool derive = false) const;
    
    // Visibility
    void setVisible(bool visible) { mVisible = visible; }
    bool isVisible() const { return mVisible; }
    
    // Render queue group
    void setRenderQueueGroup(uint8 queueID);
    uint8 getRenderQueueGroup() const;
    
    // Add to render queue when visible
    virtual void _updateRenderQueue(RenderQueue* queue) = 0;
    
    // Called when attached/detached
    virtual void _notifyAttached(Node* parent, bool isTagPoint = false);
    
    // Type identification
    virtual const String& getMovableType() const = 0;
    
protected:
    String mName;
    SceneNode* mParentNode = nullptr;
    bool mVisible = true;
    uint8 mRenderQueueID;
    
    mutable AxisAlignedBox mWorldAABB;
};
```

### 3. Entity (Mesh Instance)

```cpp
// OgreEntity.h
class _OgreExport Entity : public MovableObject {
public:
    // Create from mesh
    Entity(const String& name, const MeshPtr& mesh);
    
    // Get the mesh
    const MeshPtr& getMesh() const { return mMesh; }
    
    // Material override
    void setMaterialName(const String& name, 
                         const String& groupName = ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    
    // SubEntity access (one per submesh)
    SubEntity* getSubEntity(unsigned int index);
    SubEntity* getSubEntity(const String& name);
    
    // Animation
    AnimationState* getAnimationState(const String& name);
    AnimationStateSet* getAllAnimationStates();
    
    // LOD
    void setMeshLodBias(Real factor, ushort maxDetailIndex = 0, 
                        ushort minDetailIndex = 99);
    
    // MovableObject implementation
    const AxisAlignedBox& getBoundingBox() const override;
    void _updateRenderQueue(RenderQueue* queue) override;
    const String& getMovableType() const override;
    
private:
    MeshPtr mMesh;
    std::vector<SubEntity*> mSubEntityList;
    AnimationStateSet* mAnimationState = nullptr;
    
    // LOD
    Real mMeshLodFactorTransformed;
    ushort mMeshLodIndex;
};
```

### 4. SceneManager

```cpp
// OgreSceneManager.h
class _OgreExport SceneManager {
public:
    // Root of scene graph
    SceneNode* getRootSceneNode() { return mSceneRoot; }
    
    // Create scene nodes
    SceneNode* createSceneNode();
    SceneNode* createSceneNode(const String& name);
    void destroySceneNode(SceneNode* node);
    
    // Create entities
    Entity* createEntity(const String& entityName, const String& meshName,
                         const String& groupName = ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    void destroyEntity(Entity* ent);
    
    // Create lights
    Light* createLight(const String& name);
    void destroyLight(Light* light);
    
    // Create cameras
    Camera* createCamera(const String& name);
    void destroyCamera(Camera* cam);
    
    // Main render method
    void _renderScene(Camera* camera, Viewport* vp, bool includeOverlays);
    
    // Find visible objects
    void _findVisibleObjects(Camera* cam, VisibleObjectsBoundsInfo* visInfo);
    
    // Render queue access
    RenderQueue* getRenderQueue() { return mRenderQueue.get(); }
    
protected:
    // Scene graph root
    SceneNode* mSceneRoot;
    
    // All created objects
    std::map<String, MovableObject*> mMovableObjects;
    std::map<String, Camera*> mCameras;
    std::map<String, Light*> mLights;
    
    // Render queue for sorting
    std::unique_ptr<RenderQueue> mRenderQueue;
    
    // Current rendering state
    RenderSystem* mDestRenderSystem;
};
```

### 5. Typical Usage

```cpp
// Game code example
class MyGame {
    Ogre::SceneManager* mSceneMgr;
    
    void createScene() {
        // Create a node for our character
        Ogre::SceneNode* characterNode = 
            mSceneMgr->getRootSceneNode()->createChildSceneNode("CharacterNode");
        
        // Create entity from mesh
        Ogre::Entity* characterEntity = 
            mSceneMgr->createEntity("Character", "robot.mesh");
        
        // Attach entity to node
        characterNode->attachObject(characterEntity);
        
        // Position the node
        characterNode->setPosition(100, 0, 100);
        characterNode->setScale(2.0, 2.0, 2.0);
        
        // Create a light
        Ogre::Light* light = mSceneMgr->createLight("MainLight");
        light->setType(Ogre::Light::LT_DIRECTIONAL);
        light->setDirection(Ogre::Vector3(-1, -1, -1).normalisedCopy());
        
        Ogre::SceneNode* lightNode = 
            mSceneMgr->getRootSceneNode()->createChildSceneNode("LightNode");
        lightNode->attachObject(light);
    }
    
    void updateGame(float dt) {
        // Game logic updates node transforms
        Ogre::SceneNode* charNode = mSceneMgr->getSceneNode("CharacterNode");
        charNode->translate(mVelocity * dt);
        charNode->yaw(Ogre::Degree(mTurnSpeed * dt));
        
        // Rendering happens automatically via Root::renderOneFrame()
    }
};
```

## Render Queue System

```cpp
// OGRE sorts renderables into groups
enum RenderQueueGroupID {
    RENDER_QUEUE_BACKGROUND = 0,
    RENDER_QUEUE_SKIES_EARLY = 5,
    RENDER_QUEUE_1 = 10,
    RENDER_QUEUE_2 = 20,
    // ...
    RENDER_QUEUE_MAIN = 50,  // Default for entities
    // ...
    RENDER_QUEUE_SKIES_LATE = 95,
    RENDER_QUEUE_OVERLAY = 100
};

// During _findVisibleObjects, each MovableObject adds to queue
void Entity::_updateRenderQueue(RenderQueue* queue) {
    // Add each SubEntity as a Renderable
    for (SubEntity* sub : mSubEntityList) {
        if (sub->isVisible()) {
            queue->addRenderable(sub, mRenderQueueID);
        }
    }
}
```

## Data Flow

```
Game Update                  SceneManager                 RenderSystem
    │                            │                            │
    │ node.setPosition()         │                            │
    ├───────────────────────────►│                            │
    │                            │ Mark transform dirty       │
    │                            │                            │
    │ root.renderOneFrame()      │                            │
    ├───────────────────────────►│                            │
    │                            │ _findVisibleObjects()      │
    │                            │ Update world transforms    │
    │                            │ Frustum culling            │
    │                            │ Add to RenderQueue         │
    │                            │                            │
    │                            │ _renderScene()             │
    │                            ├───────────────────────────►│
    │                            │                            │ Sort queue
    │                            │                            │ Set states
    │                            │                            │ Draw calls
```

## Key Design Principles

1. **Scene Graph Hierarchy**: Transforms inherit through node tree
2. **Separation of Data/Transform**: MovableObjects hold data, Nodes hold transforms
3. **Factory Pattern**: SceneManager creates all objects
4. **Render Queue**: Automatic sorting and batching
5. **Plugin Architecture**: Swappable RenderSystems

## Strengths

- Intuitive scene hierarchy
- Well-documented, mature codebase
- Easy to learn OOP approach
- Good for traditional 3D games
- Flexible render queue system

## Weaknesses

- Single-threaded by design
- Object-oriented overhead
- Cache-unfriendly data layout
- String-based lookups
- Complex inheritance hierarchy

## When to Use

- Traditional 3D game development
- Projects prioritizing simplicity
- Need for proven, stable architecture
- Educational purposes
- Rapid prototyping

## References

- [OGRE 3D Manual](https://ogrecave.github.io/ogre/api/latest/)
- [OGRE GitHub Repository](https://github.com/OGRECave/ogre)
- [OGRE Wiki](https://wiki.ogre3d.org/)
- Book: "Pro OGRE 3D Programming" by Gregory Junker
