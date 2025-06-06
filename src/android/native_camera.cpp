#include <android/native_camera.h>
#include <camera/NdkCaptureRequest.h>

#include <string>
#include <logger.hpp>


namespace {
    NativeCamera::CameraInfo getCamInfo(const std::string& id, ACameraMetadata* metadata_obj) {
        NativeCamera::CameraInfo cam_info{};
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
        using Res = std::pair<int, int>;
        std::vector<Res> resolutions;
        for (int i = 0; i < entry.count; i += 4)
        {
            // We are only interested in output streams, so skip input stream
            int32_t input = entry.data.i32[i + 3];
            if (input)
                continue;

            int32_t format = entry.data.i32[i + 0];
            if (format == AIMAGE_FORMAT_YUV_420_888) {
                resolutions.push_back(std::pair<int, int>{entry.data.i32[i + 1], entry.data.i32[i + 2]});
            }
        }
        std::sort(resolutions.begin(), resolutions.end(), [](const Res&a, const Res& b){
            return a.first < b.first;
        });
        auto mid_res = resolutions.size() / 2;
        cam_info.width = resolutions[mid_res].first;
        cam_info.height = resolutions[mid_res].second;
        LOGI("width ", cam_info.width, ", height ", cam_info.height);
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
    int looperCallbackFunc(int fd, int events, void* data) {

    }
}

void NativeCamera::rotationStop() {
    ASensorEventQueue_disableSensor(sensor_queue_, accelerometer_);
    ASensorManager_destroyEventQueue(sensor_manager_, sensor_queue_);
}

void NativeCamera::rotation() {
    sensor_manager_ = ASensorManager_getInstance();
    accelerometer_ = ASensorManager_getDefaultSensor(sensor_manager_, ASENSOR_TYPE_ACCELEROMETER);
    ALooper* looper = ALooper_prepare(0);
    sensor_queue_ = ASensorManager_createEventQueue(sensor_manager_, looper, 1, looperCallbackFunc, nullptr);
    ASensorEventQueue_enableSensor(sensor_queue_, accelerometer_);
    ASensorEventQueue_setEventRate(sensor_queue_, accelerometer_, 100000);
    while(track_sensor_) {
        ASensorEvent event;
        auto result = ASensorEventQueue_getEvents(sensor_queue_, &event, 1);
        while(0 >= result) {
            result = ASensorEventQueue_getEvents(sensor_queue_, &event, 1);
        }
        int rot{};
        if(event.type == ASENSOR_TYPE_ACCELEROMETER) {
            auto& accel = event.acceleration;
            auto ax = event.acceleration.x;
            auto ay = event.acceleration.y;
            auto az = event.acceleration.z;
            if(std::abs(ax) > std::abs(ay)) {
                rot = ax > 0 ? 90 : 270;
            } else {
                rot = ay > 0 ? 0 : 180;
            }
        }
        //LOGI("calculated rotation ", rot);
        if (rot != rot_) {
            rot_ = rot;
            LOGI("rotation: ", rot_);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

//    ACameraMetadata* metadata_obj;
//    ACameraManager_getCameraCharacteristics(cam_manager_, camera_.id.c_str(), &metadata_obj);
//    ACameraMetadata_const_entry entry = { 0 };
//    auto status =ACameraMetadata_getConstEntry(metadata_obj,
//                                          ACAMERA_SENSOR_ORIENTATION,
//                                          &entry);
//    if (status != ACAMERA_OK) {
//        throw std::runtime_error("no camera orientation property");
//    }
//    rot = entry.data.i32[0];
//    ACameraMetadata_free(metadata_obj);
}

NativeCamera::CameraInfo NativeCamera::getCameraId() const {

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
        for (int tag_idx = 0; tag_idx < count; ++tag_idx)
        {
            const auto& tag = tags[tag_idx];
            // We are interested in entry that describes the facing of camera
            if (ACAMERA_LENS_FACING ==tag) {
                ACameraMetadata_const_entry lensInfo = {0};
                ACameraMetadata_getConstEntry(metadata_obj, tag, &lensInfo);

                auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(
                        lensInfo.data.u8[0]);

                // Found a back-facing camera
                if (facing == ACAMERA_LENS_FACING_BACK && !front_view_
                || facing == ACAMERA_LENS_FACING_FRONT && front_view_) {
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


NativeCamera::NativeCamera(bool front_view) {
    front_view_ = front_view;
    cam_manager_ = ACameraManager_create();
    camera_ = getCameraId();
    auto cam_status = ACameraManager_openCamera(cam_manager_,
                              camera_.id.c_str(),
                              &camera_callbacks,
                              &camera_device_);
    if(cam_status != ACAMERA_OK || !camera_device_) {
        throw std::runtime_error("cannot open camera!");
    }
    constexpr int32_t MAX_BUF_COUNT = 6;
    auto status = AImageReader_new(camera_.width, camera_.height,
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
                                       TEMPLATE_RECORD,
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
    uint8_t focus_mode = ACAMERA_CONTROL_AF_MODE_CONTINUOUS_VIDEO;
    cam_status = ACaptureRequest_setEntry_u8(capture_request_, ACAMERA_CONTROL_AF_MODE, 1, &focus_mode);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot set autofocus");
    }
    uint8_t edge_mode = ACAMERA_EDGE_MODE_FAST;
    cam_status = ACaptureRequest_setEntry_u8(capture_request_, ACAMERA_EDGE_MODE, 1, &edge_mode);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot set edge mode");
    }
    uint8_t pixel_resolution = ACAMERA_SENSOR_PIXEL_MODE_MAXIMUM_RESOLUTION;
    cam_status = ACaptureRequest_setEntry_u8(capture_request_, ACAMERA_SENSOR_PIXEL_MODE, 1, &pixel_resolution);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot set pixel mode");
    }
    int32_t fps[] = {1, 1};
    cam_status = ACaptureRequest_setEntry_i32(capture_request_, ACAMERA_CONTROL_AE_TARGET_FPS_RANGE, 2, fps);
    if(cam_status != ACAMERA_OK) {
        throw std::runtime_error("cannot set fps");
    }
    sensor_thread_ = std::thread([this](){
        rotation();
    });
}

NativeCamera::~NativeCamera() {
    if(cam_manager_) {
        ACameraManager_delete(cam_manager_);
    }
    if(sensor_thread_.joinable()) {
        track_sensor_ = false;
        sensor_thread_.join();
        rotationStop();
    }
}

cv::Mat NativeCamera::getImage() const {
    AImage *image = nullptr;
    auto status = AImageReader_acquireLatestImage(image_reader_, &image);
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
    int32_t image_pixelstrides[2];
    AImage_getPlanePixelStride(image, 1, &image_pixelstrides[0]);
    AImage_getPlanePixelStride(image, 2, &image_pixelstrides[1]);
    int32_t y_stride{0};
    AImage_getPlaneRowStride(image, 0, &y_stride);
    int32_t uv_stride1{0};
    AImage_getPlaneRowStride(image, 1, &uv_stride1);
    int32_t uv_stride2{0};
    AImage_getPlaneRowStride(image, 2, &uv_stride2);

    uint8_t *y_pixel{nullptr}, *uv_pixel1{nullptr}, *uv_pixel2{nullptr};
    int32_t y_len{0}, uv_len1{0}, uv_len2{0};
    AImage_getPlaneData(image, 0, &y_pixel, &y_len);
    AImage_getPlaneData(image, 1, &uv_pixel1, &uv_len1);
    AImage_getPlaneData(image, 2, &uv_pixel2, &uv_len2);
    cv::Size actual_size(src_width, src_height);
    cv::Size half_size(src_width / 2, src_height / 2);

    cv::Mat y(actual_size, CV_8UC1, y_pixel, y_stride);
    long addr_diff = uv_pixel2 - uv_pixel1;
    cv::Mat uv, uv1, uv2;
    if(src_width > uv_stride1) {
         uv1 = cv::Mat(half_size, CV_8UC1, uv_pixel1);
         uv2 = cv::Mat(half_size, CV_8UC1, uv_pixel2);
         cv::merge(std::vector<cv::Mat>{uv1, uv2}, uv);
    } else {
         uv1 = cv::Mat(half_size, CV_8UC2, uv_pixel1, uv_stride1);
         uv2 = cv::Mat(half_size, CV_8UC2, uv_pixel2, uv_stride2);
         uv = addr_diff > 0 ? uv1 : uv2;
    }
    cv::Mat rgba_img_;
    cvtColorTwoPlane(y, uv, rgba_img_, addr_diff > 0 ? cv::COLOR_YUV2RGBA_NV12 : cv::COLOR_YUV2RGBA_NV21);
    AImage_delete(image);

    if(front_view_) {
        if(rot_ == 90 || rot_ == 270) {
            if(rot_ != 90) {
                //LOGI("image flip vertically");
                cv::flip(rgba_img_, rgba_img_, 0);
            } else {
                cv::flip(rgba_img_, rgba_img_, 1);
            }
        } else {
            if (rot_ != 180) {
                //LOGI("image transpose");
                rgba_img_ = rgba_img_.t();
                cv::flip(rgba_img_, rgba_img_, 0);
                cv::flip(rgba_img_, rgba_img_, 1);
            } else {
                cv::flip(rgba_img_, rgba_img_, 1);
            }
        }
        //LOGI("image flip horizontally");
    } else {
        //LOGI("new image");
        if(rot_ == 90 || rot_ == 270) {
            if(rot_ == 270) {
                //LOGI("image flip vertically");
                cv::flip(rgba_img_, rgba_img_, 0);
            }
        } else {
            if(rot_ != 180) {
                //LOGI("image transpose");
                rgba_img_ = rgba_img_.t();
            }
        }
        if(rot_ != 90 && rot_ != 180) {
            //LOGI("image flip horizontally");
            cv::flip(rgba_img_, rgba_img_, 1);
        }
    }
    return rgba_img_;
}
