#pragma once

#include <icamera.h>

#include <opencv2/video.hpp>

class CVCamera: public ICamera {
public:
    CVCamera();
    cv::Mat getImage() const override;
    void close() override;
private:
    mutable cv::VideoCapture capture_;
};

class CameraBuilder: public ICameraBuilder{
public:
    std::shared_ptr<ICamera> open(bool frontal_view) override {
        return std::make_shared<CVCamera>();
    }
};
