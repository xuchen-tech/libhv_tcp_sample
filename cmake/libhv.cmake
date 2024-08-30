include(ExternalProject)

set(LIBHV_ROOT ${CMAKE_BINARY_DIR}/3rdparty/libhv-v1.3.2)
set(LIBHV_URL https://github.com/ithewei/libhv)

set(LIBHV_CONFIGURE cd ${LIBHV_ROOT}/src/libhv-v1.3.2 && rm -rf _install build && mkdir _install build && cd build && cmake ..
    -DBUILD_EXAMPLES=ON
    -DBUILD_STATIC=OFF
    -DCMAKE_INSTALL_PREFIX=${LIBHV_ROOT}/src/libhv-v1.3.2/_install)

set(LIBHV_MAKE cd ${LIBHV_ROOT}/src/libhv-v1.3.2/build && make)
set(LIBHV_INSTALL cd ${LIBHV_ROOT}/src/libhv-v1.3.2/build && make install)

ExternalProject_Add(
    libhv-v1.3.2
    URL https://github.com/ithewei/libhv/archive/refs/tags/v1.3.2.tar.gz
    PREFIX ${LIBHV_ROOT}
    CONFIGURE_COMMAND ${LIBHV_CONFIGURE}
    BUILD_COMMAND ${LIBHV_MAKE}
    INSTALL_COMMAND ${LIBHV_INSTALL}
)