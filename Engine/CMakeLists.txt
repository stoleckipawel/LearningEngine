# Engine CMake configuration
# Layered DLL Architecture (Unreal-style)
#
# Module Dependency Hierarchy (strict layering - each layer only depends on layers below):
#
#   ┌─────────────────────────────────────────────────────────────┐
#   │                     SparkleEditor.dll                       │
#   └─────────────────────────────────────────────────────────────┘
#                               │
#   ┌─────────────────────────────────────────────────────────────┐
#   │                     SparkleEngine.dll                       │
#   │          (GameFramework, World, Components, Assets)         │
#   └─────────────────────────────────────────────────────────────┘
#                               │
#         ┌─────────────────────┼─────────────────────┐
#         ▼                     ▼                     ▼
#   ┌───────────┐    ┌────────────┐    ┌──────────────┐
#   │ Physics   │    │   Audio    │    │     AI       │
#   └───────────┘    └────────────┘    └──────────────┘
#         │                │                   │
#         └────────────────┼───────────────────┘
#                          │
#   ┌─────────────────────────────────────────────────────────────┐
#   │                   SparkleRenderer.dll                       │
#   └─────────────────────────────────────────────────────────────┘
#                               │
#   ┌─────────────────────────────────────────────────────────────┐
#   │                     SparkleRHI.dll                          │
#   └─────────────────────────────────────────────────────────────┘
#                               │
#   ┌─────────────────────────────────────────────────────────────┐
#   │                   SparklePlatform.dll                       │
#   └─────────────────────────────────────────────────────────────┘
#                               │
#   ┌─────────────────────────────────────────────────────────────┐
#   │                    SparkleCore.dll                          │
#   └─────────────────────────────────────────────────────────────┘
#

# Option to build as shared libraries (DLLs) or static libraries
option(SPARKLE_BUILD_SHARED "Build Sparkle modules as shared libraries (DLLs)" OFF)

# Add third-party libraries first (they're used by modules)
add_subdirectory(third_party/imgui)
add_subdirectory(third_party/cgltf)

# ============================================================================
# CORE MODULES (always built - the foundation of the engine)
# ============================================================================
add_subdirectory(Core)           # SparkleCore.dll     - Math, Memory, Events, Logging
add_subdirectory(Platform)       # SparklePlatform.dll - Window, Input, FileSystem
add_subdirectory(RHI)            # SparkleRHI.dll      - D3D12/Vulkan abstraction
add_subdirectory(Renderer)       # SparkleRenderer.dll - Rendering pipeline
add_subdirectory(GameFramework)  # SparkleEngine.dll   - World, Entities, Components
add_subdirectory(UI)             # SparkleUI.dll       - Widget system, Layout

# ============================================================================
# OPTIONAL MODULES (placeholders - add when implementation exists)
# ============================================================================
# Uncomment as these modules get implemented:
# add_subdirectory(Physics)      # SparklePhysics.dll    - PhysX/Jolt integration
# add_subdirectory(Animation)    # SparkleAnimation.dll  - Skeletal animation
# add_subdirectory(Audio)        # SparkleAudio.dll      - FMOD/Wwise integration
# add_subdirectory(AI)           # SparkleAI.dll         - Navigation, BehaviorTree
# add_subdirectory(Networking)   # SparkleNetworking.dll - Replication, RPCs
# add_subdirectory(Scripting)    # SparkleScripting.dll  - Lua/Mono integration
# add_subdirectory(Editor)       # SparkleEditor.dll     - Editor tools

# ============================================================================
# CONVENIENCE TARGET
# ============================================================================
# SparkleEngine interface target links all active modules for easy project setup
add_library(SparkleEngineAll INTERFACE)
target_link_libraries(SparkleEngineAll INTERFACE
    SparkleCore
    SparklePlatform
    SparkleRHI
    SparkleRenderer
    SparkleGameFramework
    SparkleUI
)

# Set compiler flags for debug builds
if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
endif()