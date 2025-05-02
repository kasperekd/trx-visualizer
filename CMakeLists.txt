cmake_minimum_required(VERSION 3.14)
project(trx_visualizer)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS ON)
option(BUILD_TEST "Build test executable" ON)

set(EXTERN_DIR ${PROJECT_SOURCE_DIR}/extern)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# OpenGL/GLEW
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# ImGui
add_subdirectory(${EXTERN_DIR}/imgui)
target_include_directories(imgui PUBLIC ${EXTERN_DIR}/imgui)

# ImPlot
add_subdirectory(${EXTERN_DIR}/implot)
target_include_directories(implot PUBLIC ${EXTERN_DIR}/implot)

# Ядро
add_library(trx_visualizer_core
    ${SRC_DIR}/visualizer_core.cpp
)
target_link_libraries(trx_visualizer_core PRIVATE imgui implot ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_include_directories(trx_visualizer_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

# API
add_library(trx_visualizer_api SHARED ${SRC_DIR}/visualizer_api.cpp)
target_link_libraries(trx_visualizer_api PRIVATE trx_visualizer_core)

# Тестовый main
if(BUILD_TEST)
    add_executable(trx_visualizer_test ${TEST_DIR}/test_main.cpp)
    target_link_libraries(trx_visualizer_test PRIVATE trx_visualizer_core)
endif()