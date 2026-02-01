# SparkleUI Module
# UI system: Widget framework, Layout, Data binding, Styles
# Dependencies: SparkleCore, SparklePlatform, SparkleRenderer
#
# This module provides the game UI system (separate from Editor UI).
# Uses imgui as the backend but provides higher-level abstractions.

file(GLOB_RECURSE SPARKLE_UI_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_UI_PRIVATE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_UI_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

if(SPARKLE_BUILD_SHARED)
    add_library(SparkleUI SHARED
        ${SPARKLE_UI_PUBLIC_HEADERS}
        ${SPARKLE_UI_PRIVATE_SOURCES}
        ${SPARKLE_UI_PRIVATE_HEADERS}
    )
    target_compile_definitions(SparkleUI PRIVATE SPARKLE_UI_EXPORTS)
else()
    add_library(SparkleUI STATIC
        ${SPARKLE_UI_PUBLIC_HEADERS}
        ${SPARKLE_UI_PRIVATE_SOURCES}
        ${SPARKLE_UI_PRIVATE_HEADERS}
    )
    target_compile_definitions(SparkleUI PUBLIC SPARKLE_STATIC)
endif()

target_include_directories(SparkleUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/ImGui
        # Engine root for module-prefixed includes
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(SparkleUI PUBLIC cxx_std_20)

# UI depends on Core, Platform, Renderer
target_link_libraries(SparkleUI 
    PUBLIC 
        SparkleCore
        SparklePlatform
        SparkleRenderer
    PRIVATE
        imgui
)

set_target_properties(SparkleUI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_UI_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_UI_PRIVATE_SOURCES} ${SPARKLE_UI_PRIVATE_HEADERS})
