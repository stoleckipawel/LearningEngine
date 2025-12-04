# Engine CMake configuration

# Collect all source files
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp
)

# Collect all header files for IDE visibility
file(GLOB_RECURSE ENGINE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h
)

# Create engine static library
add_library(Engine STATIC
  ${ENGINE_SOURCES}
  ${ENGINE_HEADERS}
)

# Add ImGui third-party library
add_subdirectory(third_party/imgui)

# Public include directory
target_include_directories(Engine 
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"  
    "${CMAKE_CURRENT_SOURCE_DIR}/src/D3D12"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core"
)

# Require C++20
target_compile_features(Engine PUBLIC cxx_std_20)


# Link libraries
target_link_libraries(Engine PUBLIC
  d3d12
  dxgi
  d3dcompiler
  dxguid
  dxcompiler
  imgui
)

# Set compiler flags for debug builds
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi")
endif()

# --- PCH setup ---
target_precompile_headers(Engine
    PRIVATE
      include/Core/PCH.h
)

# Set ENGINE_PATH environment variable for engine
set(ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(ENV{ENGINE_PATH} "${ENGINE_PATH}")