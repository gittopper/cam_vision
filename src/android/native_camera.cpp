#include <android/native_camera.h>

#include <string>

namespace {
    NativeCamera::CameraInfo getCamInfo(const std::string& id, ACameraMetadata* metadata_obj) {
        NativeCamera::CameraInfo cam_info;
        ACameraMetadata_const_entry entry = { 0 };
        camera_status_t status = ACameraMetadata_getConstEntry(metadata_obj,
                                      ACAMERA_SENSOR_INFO_EXPOSURE_TIME_RANGE, &entry);
        if (status != ACAMERA_OK) {
            throw std::runtime_error("no camera exposure property");
        }
        cam_info.id = id;
        cam_info.min_exposure = entry.data.i64[0];
        cam_info.max_exposure = entry.data.i64[1];
        status = ACameraMetadata_getConstEntry(metadata_obj,
                                      ACAMERA_SENSOR_INFO_SENSITIVITY_RANGE, &entry);
        if (status != ACAMERA_OK) {
            throw std::runtime_error("no camera sensitivity property");
        }
        cam_info.min_sensitivity = entry.data.i32[0];
        cam_info.max_sensistivity = entry.data.i32[1];
        status = ACameraMetadata_getConstEntry(metadata_obj,
                                      ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry);
        if (status != ACAMERA_OK) {
            throw std::runtime_error("no camera streaming property");
        }
        for (int i = 0; i < entry.count; i += 4)
        {
            // We are only interested in output streams, so skip input stream
            int32_t input = entry.data.i32[i + 3];
            if (input)
                continue;

            int32_t format = entry.data.i32[i + 0];
            if (format == AIMAGE_FORMAT_JPEG)
            {
                cam_info.width = entry.data.i32[i + 1];
                cam_info.height = entry.data.i32[i + 2];
            }
        }
        status =ACameraMetadata_getConstEntry(metadata_obj,
                                      ACAMERA_SENSOR_ORIENTATION,
                                      &entry);
        if (status != ACAMERA_OK) {
            throw std::runtime_error("no camera orientation property");
        }
        auto orientation = entry.data.i32[0];
        cam_info.portrait = orientation == 0 || orientation == 270;
        return cam_info;
    }
    static void onDisconnected(void* context, ACameraDevice* device)
    {

    }

    static void onError(void* context, ACameraDevice* device, int error)
    {

    }
    static void onActive(void* context, ACameraCaptureSession *session) {

    }
    static void onClosed(void* context, ACameraCaptureSession *session) {

    }
    static void onReady(void* context, ACameraCaptureSession *session) {

    }
    ACameraDevice_stateCallbacks camera_callbacks = {
            .context = nullptr,
            .onDisconnected = onDisconnected,
            .onError = onError,
    };
    ACameraCaptureSession_stateCallbacks session_callbacks = {
            .context = nullptr,
            .onClosed = onClosed,
            .onReady = onReady,
            .onActive = onActive

    };
}

NativeCamera::CameraInfo NativeCamera::getBackCameraId() const {

    ACameraIdList *cameraIds = nullptr;
    ACameraManager_getCameraIdList(cam_manager_, &cameraIds);
    CameraInfo cam_info;
    for (int i = 0; i < cameraIds->numCameras; ++i)
    {
        const char* id = cameraIds->cameraIds[i];
        ACameraMetadata* metadata_obj;
        ACameraManager_getCameraCharacteristics(cam_manager_, id, &metadata_obj);

        int32_t count = 0;
        const uint32_t* tags = nullptr;
        ACameraMetadata_getAllTags(metadata_obj, &count, &tags);

        for (int tagIdx = 0; tagIdx < count; ++tagIdx)
        {
            // We are interested in entry that describes the facing of camera
            if (ACAMERA_LENS_FACING == tags[tagIdx]) {
                ACameraMetadata_const_entry lensInfo = {0};
                ACameraMetadata_getConstEntry(metadata_obj, tags[tagIdx], &lensInfo);

                auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(
                        lensInfo.data.u8[0]);

                // Found a back-facing camera
                if (facing == ACAMERA_LENS_FACING_BACK) {
                    cam_info = getCamInfo(id, metadata_obj);
                    break;
                }
            }
        }

        ACameraMetadata_free(metadata_obj);

    }
    ACameraManager_deleteCameraIdList(cameraIds);
    return cam_info;
}


NativeCamera::NativeCamera() {
    cam_manager_ = ACameraManager_create();
    back_camera_ = getBackCameraId();
    auto cam_status = ACameraManager_openCamera(cam_manager_,
                              back_camera_.id.c_str(),
                              &camera_callbacks,
                              &camera_device_);
    if(cam_status != ACAMERA_OK || !camera_device_) {
        throw std::runtime_error("cannot open camera!");
    }
    constexpr int32_t MAX_BUF_COUNT = 4;
    auto status = AImageReader_new(back_camera_.width, back_camera_.height,
                                   AIMAGE_FORMAT_YUV_420_888,
                                   MAX_BUF_COUNT,
                                   &image_reader_);
    if(status != AMEDIA_OK || !image_reader_) {
        throw std::runtime_error("cannot create image reader");
    }
    status = AImageReader_getWindow(image_reader_, &output_native_window_);
    if(status != AMEDIA_OK) {
        throw std::runtime_error("cannot get window");
    }
    ANativeWindow_acquire(output_native_window_);
    cam_status = ACaptureSessionOutputContainer_create(&output_container_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot create session output");
    }
    cam_status = ACaptureSessionOutput_create(output_native_window_, &session_output_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot create session output");
    }

    cam_status = ACaptureSessionOutputContainer_add(output_container_, session_output_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot create output container");
    }
    cam_status = ACameraOutputTarget_create(output_native_window_, &output_target_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot create output target");
    }

    cam_status = ACameraDevice_createCaptureRequest(camera_device_,
                                       TEMPLATE_PREVIEW,
                                       &capture_request_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot create capture request");
    }


    cam_status = ACaptureRequest_addTarget(capture_request_, output_target_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot add target");
    }
    cam_status = ACameraDevice_createCaptureSession(camera_device_,
                                                    output_container_,
                                                    &session_callbacks,
                                                    &capture_session_);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot create capture session");
    }

    cam_status = ACameraCaptureSession_setRepeatingRequest(capture_session_,
                                                           nullptr,
                                                           1,
                                                           &capture_request_,
                                                           nullptr);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot set request");
    }
}
cv::Mat NativeCamera::getImage() const {
    AImage *image = nullptr;
    auto status = AImageReader_acquireNextImage(image_reader_, &image);
    if (status != AMEDIA_OK) {
        return {};
    }
    int32_t src_format = -1;
    AImage_getFormat(image, &src_format);
    if(AIMAGE_FORMAT_YUV_420_888 != src_format) {
        throw std::runtime_error("Unsupported image format for displaying");
    }

    int32_t num_src_planes{0};
    AImage_getNumberOfPlanes(image, &num_src_planes);
    if(num_src_planes != 3) {
        throw std::runtime_error("Image for display has unsupported number of planes");
    }

    int32_t src_height{0};
    AImage_getHeight(image, &src_height);
    int32_t src_width{0};
    AImage_getWidth(image, &src_width);
    int32_t y_stride{0};
    AImage_getPlaneRowStride(image, 0, &y_stride);
    int32_t uv_stride1{0};
    AImage_getPlaneRowStride(image, 1, &uv_stride1);
    int32_t uv_stride2{0};
    AImage_getPlaneRowStride(image, 1, &uv_stride2);

    uint8_t *y_pixel{nullptr}, *uv_pixel1{nullptr}, *uv_pixel2{nullptr};
    int32_t y_len{0}, uv_len1{0}, uv_len2{0};
    AImage_getPlaneData(image, 0, &y_pixel, &y_len);
    AImage_getPlaneData(image, 1, &uv_pixel1, &uv_len1);
    AImage_getPlaneData(image, 2, &uv_pixel2, &uv_len2);
    cv::Size actual_size(src_width, src_height);
    cv::Size half_size(src_width / 2, src_height / 2);

    cv::Mat y(actual_size, CV_8UC1, y_pixel, y_stride);
    cv::Mat uv1(half_size, CV_8UC2, uv_pixel1, 2 * uv_stride1);
    cv::Mat uv2(half_size, CV_8UC2, uv_pixel2, 2 * uv_stride2);
    cv::Mat rgba_img_;
    long addr_diff = uv2.data - uv1.data;
    if (addr_diff > 0) {
        cvtColorTwoPlane(y, uv1, rgba_img_, cv::COLOR_YUV2RGBA_NV12);
    } else {
        cvtColorTwoPlane(y, uv2, rgba_img_, cv::COLOR_YUV2RGBA_NV21);
    }
    AImage_delete(image);
    return rgba_img_;
}

NativeCamera::~NativeCamera() {
    if(cam_manager_) {
        ACameraManager_delete(cam_manager_);
    }
}