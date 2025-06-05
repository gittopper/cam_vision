#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo SCRIPT_DIR=$SCRIPT_DIR
THRD_PARTY_INSTALL_DIR=$SCRIPT_DIR/../../build/install/desktop
mkdir -p $THRD_PARTY_INSTALL_DIR

#sudo apt-get install libudev-dev libvorbis-dev libflac-dev

function buildOpenCV() {
    build_type=$1
    mkdir -p $THRD_PARTY_INSTALL_DIR/$build_type
    cd $SCRIPT_DIR/opencv
    mkdir -p build/$build_type/desktop
    cd build/$build_type/desktop
    cmake ../../.. \
    -DCMAKE_C_COMPILER=clang-15 \
    -DCMAKE_CXX_FLAGS=-g \
    -DCMAKE_CXX_COMPILER=clang++-15 \
    -DCMAKE_INSTALL_PREFIX=$THRD_PARTY_INSTALL_DIR/$build_type \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_ANDROID_PROJECTS=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_PROTBUF=ON \
    -DWITH_PROTOBUF=ON \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_java_bindings_generator=OFF \
    -DBUILD_opencv_js_bindings_generator=OFF \
    -DBUILD_opencv_python3=OFF \
    -DBUILD_opencv_python_bindings_generator=OFF \
    -DBUILD_opencv_python_tests=OFF \
    -DBUILD_opencv_objc_bindings_generator=OFF \
    -DBUILD_opencv_objdetect=ON \
    -DBUILD_opencv_dnn=ON \
    -DBUILD_opencv_highgui=OFF \
    -DBUILD_opencv_ml=OFF

    make -j
    cmake --install . --prefix $THRD_PARTY_INSTALL_DIR/$build_type
}

buildOpenCV Debug
buildOpenCV Release
