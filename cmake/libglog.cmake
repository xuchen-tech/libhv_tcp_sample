include(ExternalProject)

set(LIBGLOG_ROOT ${CMAKE_BINARY_DIR}/3rdparty/libglog-0.6.0)

set(LIBGLOG_CONFIGURE cd ${LIBGLOG_ROOT}/src/libglog-0.6.0 && rm -rf _install build && mkdir _install build && cd build && cmake ..
    -DBUILD_EXAMPLES=OFF
    -DBUILD_STATIC=OFF
    -DCMAKE_INSTALL_PREFIX=${LIBGLOG_ROOT}/src/libglog-0.6.0/_install)

set(LIBGLOG_MAKE cd ${LIBGLOG_ROOT}/src/libglog-0.6.0/build && make)
set(LIBGLOG_INSTALL cd ${LIBGLOG_ROOT}/src/libglog-0.6.0/build && make install)

ExternalProject_Add(
    libglog-0.6.0
    URL https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz
    PREFIX ${LIBGLOG_ROOT}
    CONFIGURE_COMMAND ${LIBGLOG_CONFIGURE}
    BUILD_COMMAND ${LIBGLOG_MAKE}
    INSTALL_COMMAND ${LIBGLOG_INSTALL}
)