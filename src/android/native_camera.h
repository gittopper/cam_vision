#pragma once

#include <string>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadataTags.h>
#include <camera/NdkCameraMetadata.h>
#include <media/NdkImageReader.h>

#include <icamera.h>
#include <android/sensor.h>
#include <thread>

class NativeCamera: public ICamera {
  public:
    NativeCamera(bool front_view);
    ~NativeCamera();
    struct CameraInfo{
        std::string id;
        int width;
        int height;
        bool portrait;
        int64_t min_exposure;
        int64_t max_exposure;
        int64_t min_sensitivity;
        int64_t max_sensistivity;
    };
    cv::Mat getImage() const override;
    void close() override;
  private:
    CameraInfo getCameraId() const;
    void rotation();
    void rotationStop();
    bool front_view_;
    CameraInfo camera_;
    ACameraManager *cam_manager_ = nullptr;
    ACameraDevice* camera_device_ = nullptr;
    AImageReader* image_reader_ = nullptr;
    ANativeWindow *output_native_window_ = nullptr;
    ACaptureSessionOutputContainer* output_container_ = nullptr;
    ACameraCaptureSession* capture_session_ = nullptr;
    ACaptureRequest*capture_request_ = nullptr;
    ACaptureSessionOutput* session_output_ = nullptr;
    ACameraOutputTarget* output_target_ = nullptr;
    int rot_ = 0;
    bool track_sensor_ = true;
    std::thread sensor_thread_;
    ASensorManager* sensor_manager_ = nullptr;
    const ASensor* accelerometer_ = nullptr;
    ALooper* looper_ = nullptr;
    ASensorEventQueue* sensor_queue_ = nullptr;
};

class CameraBuilder: public ICameraBuilder{
public:
    std::shared_ptr<ICamera> open(bool frontal_view) override {
        return std::make_shared<NativeCamera>(frontal_view);
    }
};
