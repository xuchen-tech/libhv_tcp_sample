cmake_minimum_required(VERSION 3.10)

project(libhv_communication)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin_gen)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libhv.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libglog.cmake)

include_directories(${LIBHV_ROOT}/src/libhv-v1.3.2/_install/include/hv
    ${LIBGLOG_ROOT}/src/libglog-0.6.0/_install/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common)

link_directories(${LIBHV_ROOT}/src/libhv-v1.3.2/_install/lib
    ${LIBGLOG_ROOT}/src/libglog-0.6.0/_install/lib)

file(GLOB_RECURSE SRC_COMMON_FILES "${PROJECT_SOURCE_DIR}/common/*.cpp")
file(GLOB_RECURSE SRC_SERVER_FILES "${PROJECT_SOURCE_DIR}/server/*.cpp")
file(GLOB_RECURSE SRC_CLIENT_FILES "${PROJECT_SOURCE_DIR}/client/*.cpp")


add_executable(libhv_server ${SRC_SERVER_FILES} ${SRC_COMMON_FILES} )
add_executable(libhv_client ${SRC_CLIENT_FILES} ${SRC_COMMON_FILES})

target_link_libraries(libhv_server hv glog pthread)
target_link_libraries(libhv_client hv glog pthread)

add_dependencies(libhv_server libhv-v1.3.2 libglog-0.6.0)
add_dependencies(libhv_client libhv-v1.3.2 libglog-0.6.0)