#pragma once

#include <opencv2/opencv.hpp>

class ICamera{
public:
    virtual ~ICamera() = default;
    virtual cv::Mat getImage() const = 0;
};
