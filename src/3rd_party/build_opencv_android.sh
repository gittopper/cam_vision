#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo SCRIPT_DIR=$SCRIPT_DIR
NDK_PATH=/home/stanislav/Android/Sdk/ndk/29.0.13113456/
THRD_PARTY_INSTALL_DIR=$SCRIPT_DIR/../../build/install/android/

#sudo apt-get install libudev-dev libvorbis-dev libflac-dev

function buildABI() {
    abi=$1
    build_type=$2
    echo abi=$abi
    cd $SCRIPT_DIR/opencv/build/$build_type
    mkdir $abi
    cd $abi

    mkdir $THRD_PARTY_INSTALL_DIR/$build_type/$abi
    cmake ../../.. \
    -DCMAKE_INSTALL_PREFIX=$THRD_PARTY_INSTALL_DIR/$abi \
    -DANDROID_ABI=$abi \
    -DANDROID_NATIVE_API_LEVEL=29 \
    -DANDROID_PLATFORM=android-21 \
    -DCLANG_DEFAULT_CXX_STDLIB=libc++ \
    -DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_ANDROID_NDK=${NDK_PATH} \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=$build_type \
    -DANDROID_SDK_ROOT=${NDK_PATH} \
    -DBUILD_ANDROID_PROJECTS=OFF \
    -DBUILD_PROTBUF=OFF \
    -DWITH_PROTOBUF=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_java_bindings_generator=OFF \
    -DBUILD_opencv_js_bindings_generator=OFF \
    -DBUILD_opencv_python3=OFF \
    -DBUILD_opencv_python_bindings_generator=OFF \
    -DBUILD_opencv_python_tests=OFF \
    -DBUILD_opencv_objc_bindings_generator=OFF \
    -DBUILD_opencv_objdetect=OFF \
    -DBUILD_opencv_dnn=OFF \
    -DBUILD_opencv_highgui=OFF \
    -DBUILD_opencv_ml=OFF \
    -G "Unix Makefiles"

    make -j
    cmake --install . --prefix $THRD_PARTY_INSTALL_DIR/$build_type/$abi
}

function builFlavours() {
    flavour=$1
    mkdir -p $THRD_PARTY_INSTALL_DIR/$flavour
    mkdir -p $SCRIPT_DIR/opencv/build/$flavour
    buildABI 'x86_64' $flavour
    buildABI 'armeabi-v7a' $flavour
    buildABI 'arm64-v8a' $flavour
}

builFlavours Debug
#builFlavours Release
#builFlavours RelWithDebInfo
