cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)
project(test_opencl CXX)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/third_party")

find_package(OpenCL REQUIRED)

include_directories(${THIRD_PARTY}/opencv/include ${OpenCL_INCLUDE_DIRS})
link_directories(PRIVATE ${THIRD_PARTY}/opencv/lib)
link_libraries(${OpenCL_LIBRARIES} opencv_world430)

add_library(utils utils/utils.cpp)
link_libraries(utils) # Link to all next targets

add_executable(vector_ops
    vector_ops/main.cpp
)

add_executable(benchmark
    benchmark/main.cpp
)

add_executable(blur_image
    blur_image/main.cpp
)
