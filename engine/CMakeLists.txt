# Engine CMake configuration

# Collect all source files
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp    
)

# Collect all header files for IDE visibility
file(GLOB_RECURSE ENGINE_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h
)

# Create engine static library
add_library(SparkleEngine STATIC
  ${ENGINE_SOURCES}
  ${ENGINE_HEADERS}
)

# Add ImGui third-party library
add_subdirectory(third_party/imgui)

# Include directories
# Public: exposed to consumers (samples, games)
# Private: internal engine modules organized by responsibility
target_include_directories(SparkleEngine 
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform"
    # RHI abstraction layer
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI"
    # D3D12 backend
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/D3D12"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/D3D12/Descriptors"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/D3D12/Resources"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/D3D12/Pipeline"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/D3D12/Shaders"
    # Vulkan backend (placeholder)
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/Vulkan"
    # High-level modules
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Resources"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Scene"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/UI"
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
      src/Core/PCH.h
)

# Set ENGINE_PATH environment variable for engine
set(ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(ENV{ENGINE_PATH} "${ENGINE_PATH}")