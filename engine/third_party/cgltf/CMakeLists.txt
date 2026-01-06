# cgltf — Single-header glTF 2.0 parser
# https://github.com/jkuhlmann/cgltf
#
# Header-only library. Define CGLTF_IMPLEMENTATION in exactly one .cpp file
# before including cgltf.h to generate the implementation.

add_library(cgltf INTERFACE)

target_include_directories(cgltf INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Silence common warnings in third-party C code when included
if(MSVC)
    target_compile_options(cgltf INTERFACE
        /wd4996  # Deprecated functions
    )
endif()
