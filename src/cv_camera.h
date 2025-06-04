#pragma once

#include <icamera.h>

#include <opencv2/video.hpp>

class CVCamera: public ICamera {
public:
    CVCamera();
    cv::Mat getImage() const override;
private:
    mutable cv::VideoCapture capture_;
};
