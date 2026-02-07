# SampleScenes project CMake configuration

# Add executable target
add_executable(SampleScenes
    src/main.cpp
)

# Require C++20
target_compile_features(SampleScenes PRIVATE cxx_std_20)

# Link against engine modules explicitly
# SparkleApplication brings in GameFramework + Renderer + all their dependencies
target_link_libraries(SampleScenes PRIVATE
    SparkleApplication    # App class (ties GameFramework + Renderer together)
    SparkleUI             # ImGui integration
)

# Include third-party headers if needed directly
target_include_directories(SampleScenes PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/third_party
)

# Configure Visual Studio debugger to launch from project directory.
# Path discovery uses marker files, so we just need the working directory set correctly.
if(MSVC)
    set_target_properties(SampleScenes PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
