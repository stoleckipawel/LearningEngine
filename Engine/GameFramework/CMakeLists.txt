# SparkleGameFramework Module (SparkleEngine.dll)
# Game framework: World, Entities, Components, Scene, Camera, Mesh, Assets
# Dependencies: SparkleCore, SparklePlatform, SparkleRHI, SparkleRenderer
#
# This corresponds to "SparkleEngine.dll" in the Unreal-style architecture.
# It contains the high-level game systems that build on top of the renderer.

file(GLOB_RECURSE SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_GAMEFRAMEWORK_PRIVATE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_GAMEFRAMEWORK_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

if(SPARKLE_BUILD_SHARED)
    add_library(SparkleGameFramework SHARED
        ${SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS}
        ${SPARKLE_GAMEFRAMEWORK_PRIVATE_SOURCES}
        ${SPARKLE_GAMEFRAMEWORK_PRIVATE_HEADERS}
    )
    target_compile_definitions(SparkleGameFramework PRIVATE SPARKLE_ENGINE_EXPORTS)
else()
    add_library(SparkleGameFramework STATIC
        ${SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS}
        ${SPARKLE_GAMEFRAMEWORK_PRIVATE_SOURCES}
        ${SPARKLE_GAMEFRAMEWORK_PRIVATE_HEADERS}
    )
    target_compile_definitions(SparkleGameFramework PUBLIC SPARKLE_STATIC)
endif()

target_include_directories(SparkleGameFramework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Scene
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Scene/Camera
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Application
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        # Engine root for module-prefixed includes
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        # Core module subdirectories for direct includes
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Events
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Math
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Time
        # Platform module subdirectories for InputSystem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../Platform/Public/Input
        # RHI module subdirectories for transitive includes from Renderer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../RHI/Public/D3D12
        ${CMAKE_CURRENT_SOURCE_DIR}/../RHI/Public/D3D12/Shaders
        ${CMAKE_CURRENT_SOURCE_DIR}/../RHI/Public/D3D12/Resources
        ${CMAKE_CURRENT_SOURCE_DIR}/../RHI/Public/D3D12/Descriptors
        # Subdirectories for internal includes
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Scene
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Scene/Camera
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Scene/Primitives/Basic
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Scene/Primitives/Polyhedra
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Assets
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Application
        # Public subdirectories for direct includes from Private code
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Application
)

target_compile_features(SparkleGameFramework PUBLIC cxx_std_20)

# Precompiled header (per-module PCH)
target_precompile_headers(SparkleGameFramework PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

# GameFramework depends on lower-level modules
target_link_libraries(SparkleGameFramework 
    PUBLIC 
        SparkleCore
        SparklePlatform
        SparkleRHI
        SparkleRenderer
    PRIVATE
        cgltf
)

set_target_properties(SparkleGameFramework PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_RUNTIME_PRIVATE_SOURCES} ${SPARKLE_RUNTIME_PRIVATE_HEADERS})
