# SampleScenes project CMake configuration

# Add executable target
add_executable(SampleScenes
    src/main.cpp
)

# Include engine public headers and third-party (if needed)
target_include_directories(SampleScenes PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_SOURCE_DIR}/engine/third_party
)

# Require C++20
target_compile_features(SampleScenes PRIVATE cxx_std_20)

# Link against engine static library
target_link_libraries(SampleScenes PRIVATE SparkleEngine)

# Configure Visual Studio debugger to launch from project directory.
# Path discovery uses marker files, so we just need the working directory set correctly.
if(MSVC)
    set_target_properties(SampleScenes PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
