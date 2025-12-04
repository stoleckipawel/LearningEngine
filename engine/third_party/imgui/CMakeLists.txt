cmake_minimum_required(VERSION 3.20)

# ImGui static library (core + DX12/Win32 backends)

# Auto-include core sources present in the folder (reconfigure when files change)
file(GLOB IMGUI_CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Auto-include backend sources (any new backend .cpp files)
file(GLOB IMGUI_BACKEND_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/*.cpp
)

add_library(imgui STATIC
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Link required system libraries for Win32 + DX12 backends
target_link_libraries(imgui PUBLIC
    d3d12
    dxgi
    dxguid
    user32
    imm32
    winmm
)

target_compile_features(imgui PUBLIC cxx_std_20)
