cmake_minimum_required(VERSION 3.18)

option(ANDROID_REGIME "android regime" OFF)

project(CamVision VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/3rd_party)


set(CAMVIS_SOURCES
        src/rendering/renderer.h
        src/rendering/glesrenderer.cpp
        src/rendering/glesrenderer.h
        src/cv_camera.cpp
        src/cv_camera.h
        src/engine.cpp
        src/engine.h
        src/math/mat.cpp
        src/math/mat44.cpp
        src/math/vector.cpp
)

if(ANDROID_REGIME)

    list(APPEND CAMVIS_SOURCES
        src/android/native_camera.cpp
        src/android/native_camera.h
        src/android/androidresourceloader.cpp
        src/android/androidresourceloader.h
        src/android/cam_vision.cpp
    )
    set(TARGET cam_vision)
    message(ANDROID_ABI=${ANDROID_ABI})
    add_library(${TARGET} SHARED ${CAMVIS_SOURCES})
    target_link_libraries(${TARGET} log android EGL GLESv2 OpenSLES
        dl  GLESv1_CM png_static zlibstatic freetype)
    set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_NAME ${TARGET})
    target_include_directories(${TARGET} PRIVATE src)
    set(OpenCV_DIR ${CMAKE_CURRENT_LIST_DIR}/build/install/android/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}/sdk/native/jni)
    message(OpenCV_DIR=${OpenCV_DIR})
    find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
    find_library(camera-lib camera2ndk)
    find_library(media-lib mediandk)
    find_library(android-lib android)
    target_link_libraries(${TARGET} opencv_core opencv_imgproc opencv_video opencv_videoio
        ${camera-lib} ${media-lib} ${android-lib})
else()

    file(COPY src/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND CAMVIS_SOURCES
        src/desktop/fileresourceloader.cpp
        src/desktop/fileresourceloader.h
    )
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets Multimedia REQUIRED)

    add_executable(cam_vision
            QT/src/main.cpp
            QT/src/mainwindow.h
            QT/src/mainwindow.cpp
            ${CAMVIS_SOURCES}
    )

    target_include_directories(cam_vision PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/QT/src
            src
        )
    set(OpenCV_DIR ${CMAKE_CURRENT_LIST_DIR}/build/install/desktop/${CMAKE_BUILD_TYPE}/lib/cmake)
    find_package(Eigen3)
    find_package (OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

    target_link_libraries(cam_vision Qt6::Widgets Qt6::OpenGL
        opencv_core opencv_imgproc opencv_video  opencv_videoio
        Qt6::OpenGLWidgets Qt6::Multimedia -lGL png_static zlibstatic freetype)

endif()
