# __PROJECT_NAME__ project CMake configuration

# Add executable target
add_executable(__PROJECT_NAME__
    src/main.cpp
)

# Require C++20
target_compile_features(__PROJECT_NAME__ PRIVATE cxx_std_20)

# Link against engine modules explicitly
# Only link what you need - remove modules you don't use to reduce build time
target_link_libraries(__PROJECT_NAME__ PRIVATE
    SparkleCore           # Math, Events, Logging
    SparklePlatform       # Window, Input
    SparkleRHI            # D3D12 backend
    SparkleRenderer       # High-level rendering
    SparkleGameFramework  # Scene, Camera, Primitives
    SparkleUI             # ImGui integration
)

# Include third-party headers if needed directly
target_include_directories(__PROJECT_NAME__ PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/third_party
)

# Configure Visual Studio debugger to launch from project directory.
# Path discovery uses marker files, so we just need the working directory set correctly.
if(MSVC)
    set_target_properties(__PROJECT_NAME__ PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
