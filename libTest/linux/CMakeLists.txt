cmake_minimum_required(VERSION 3.28)
project(libTest C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


include_directories(
    ../../platform/linuxbsd/hook/
)
# 强制包含调试符号
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

# 禁用优化
add_compile_options(
    $<$<CONFIG:Debug>:-fno-inline>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
)
add_executable(libTest
        main.cpp
)
add_library(godot-lib STATIC IMPORTED)
set_target_properties(godot-lib PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/godot.linuxbsd.template_debug.dev.x86_64)
target_link_libraries(libTest
        glfw
        EGL
        GLESv2
        godot-lib
)