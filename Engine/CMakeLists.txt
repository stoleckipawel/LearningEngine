# Engine CMake configuration

# Collect all source files
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp    
)

# Collect all header files for IDE visibility
file(GLOB_RECURSE ENGINE_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Include/**/*.h
)

# Create engine static library
add_library(SparkleEngine STATIC
  ${ENGINE_SOURCES}
  ${ENGINE_HEADERS}
)

# Add third-party libraries
add_subdirectory(third_party/imgui)
add_subdirectory(third_party/cgltf)

# Include directories
# Public: exposed to consumers (projects, games)
# Private: internal engine modules organized by responsibility
target_include_directories(SparkleEngine 
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Src"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Application"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Assets"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Diagnostics"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Events"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Hash"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Math"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Strings"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Time"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Core/Input"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Platform/Input"
    # D3D12 RHI
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI/D3D12"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI/D3D12/Descriptors"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI/D3D12/Resources"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI/D3D12/Pipeline"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI/D3D12/Shaders"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/RHI/D3D12/Samplers"
    # High-level modules
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Renderer"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/Scene"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/GameEngine/Camera"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/UI"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

# Require C++20
target_compile_features(SparkleEngine PUBLIC cxx_std_20)


# Link libraries
target_link_libraries(SparkleEngine PUBLIC
  d3d12
  dxgi
  d3dcompiler
  dxguid
  dxcompiler
  imgui
  cgltf
)

# Set compiler flags for debug builds.
# Use MSVC-style flags when building with Visual Studio/MSVC (including clang-cl),
# otherwise use GNU/Clang-style flags.
if (MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
endif()

# --- PCH setup ---
target_precompile_headers(SparkleEngine
    PRIVATE
      Src/Core/PCH.h
)