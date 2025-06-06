#pragma once

#include <opencv2/opencv.hpp>

class ICamera{
public:
    virtual ~ICamera() = default;
    virtual cv::Mat getImage() const = 0;
    virtual void close() = 0;
};

class ICameraBuilder{
public:
    virtual ~ICameraBuilder() = default;
    virtual std::shared_ptr<ICamera> open(bool frontal_view) = 0;
};
