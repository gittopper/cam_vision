#include <cv_camera.h>

#include <opencv2/imgproc.hpp>

CVCamera::CVCamera():
    capture_(0)
{}

cv::Mat CVCamera::getImage() const {
    cv::Mat frame;
    capture_ >> frame;
    if (!frame.empty()) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGBA, 4);
    }
    return frame;
}
