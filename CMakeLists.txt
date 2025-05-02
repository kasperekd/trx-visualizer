cmake_minimum_required(VERSION 3.14)
project(trx_visualizer)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS ON)
option(BUILD_TEST "Build test executable" ON)

set(EXTERN_DIR ${PROJECT_SOURCE_DIR}/extern)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# OpenGL/GLEW
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${INCLUDE_DIR})

if(NOT EXISTS "${EXTERN_DIR}/imgui/")
    message(FATAL_ERROR "ImGui submodule not found. Did you run 'git submodule update --init --recursive'?")
endif()
if(NOT EXISTS "${EXTERN_DIR}/implot/")
    message(FATAL_ERROR "ImPlot submodule not found. Did you run 'git submodule update --init --recursive'?")
endif()

add_library(imgui STATIC
    ${EXTERN_DIR}/imgui/imgui.cpp
    ${EXTERN_DIR}/imgui/imgui_draw.cpp
    ${EXTERN_DIR}/imgui/imgui_tables.cpp
    ${EXTERN_DIR}/imgui/imgui_widgets.cpp
    ${EXTERN_DIR}/imgui/backends/imgui_impl_sdl2.cpp
    ${EXTERN_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${EXTERN_DIR}/imgui)
target_compile_options(imgui PRIVATE -fPIC)

add_library(implot STATIC
    ${EXTERN_DIR}/implot/implot.cpp
    ${EXTERN_DIR}/implot/implot_items.cpp
)
target_include_directories(implot PUBLIC ${EXTERN_DIR}/implot)
target_link_libraries(implot PRIVATE imgui)
target_compile_options(implot PRIVATE -fPIC)

# core
add_library(trx_visualizer_core SHARED
    ${SRC_DIR}/visualizer_core.cpp
    ${SRC_DIR}/visualizer_context.cpp
)
target_link_libraries(trx_visualizer_core PRIVATE imgui implot ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_include_directories(trx_visualizer_core PUBLIC ${INCLUDE_DIR})

# API
add_library(trx_visualizer_api SHARED ${SRC_DIR}/visualizer_api.cpp)
target_link_libraries(trx_visualizer_api PRIVATE trx_visualizer_core)

# test main
if(BUILD_TEST)
    add_executable(trx_visualizer_test ${TEST_DIR}/test_main.cpp)
    target_link_libraries(trx_visualizer_test PRIVATE trx_visualizer_api trx_visualizer_core ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
endif()