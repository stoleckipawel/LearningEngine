# HelloWorld project CMake configuration

# Add executable target
add_executable(HelloWorld
    src/main.cpp
)

# Require C++20
target_compile_features(HelloWorld PRIVATE cxx_std_20)

# Link against engine modules (SparkleEngineAll links all active modules)
target_link_libraries(HelloWorld PRIVATE SparkleEngineAll)

# Include third-party headers if needed directly
target_include_directories(HelloWorld PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/third_party
)

# Configure Visual Studio debugger to launch from project directory.
# Path discovery uses marker files, so we just need the working directory set correctly.
if(MSVC)
    set_target_properties(HelloWorld PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
