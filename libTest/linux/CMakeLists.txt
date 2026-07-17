cmake_minimum_required(VERSION 3.28)
project(libTest C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DIMGUI_IMPL_OPENGL_ES3)

include_directories(
    ../../platform/embedded/
    ./imgui
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
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_impl_opengl3.cpp
        imgui/imgui_impl_glfw.cpp
)
set(GODOT_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libgodot.embedded.template_debug.dev.x86_64.so)
add_library(godot-lib SHARED IMPORTED)
set_target_properties(godot-lib PROPERTIES IMPORTED_LOCATION ${GODOT_LIB_PATH})
set_target_properties(libTest PROPERTIES BUILD_RPATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
target_link_libraries(libTest
        glfw
        EGL
        GLESv2
        godot-lib
)
