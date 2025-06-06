#pragma once

#include <rendering/glesrenderer.h>
#include <sprite.h>
#include <mutex>
#include <icamera.h>
#include <thread>
#include <opencv2/objdetect/face.hpp>

class Engine{
public:
    Engine();
    ~Engine();
    void setup(int w, int h, ResourceLoader* loader);
    void resize(int w, int h);
    void step();
    Renderer& renderer() {
        return renderer_;
    }
    void setCamBuilder(std::shared_ptr<ICameraBuilder> builder) {
        cam_builder_ = std::move(builder);
    }
    void tap(int x, int y);
    void pause(){}
    void resume(){}
private:
    Camera camera_;
    std::shared_ptr<ICameraBuilder> cam_builder_;
    std::shared_ptr<ICamera> web_cam_;
    std::thread cam_thread_;
    GLESRenderer renderer_;
    std::shared_ptr<Sprite> cam_image_;
    std::mutex m_;
    bool do_capture_ = true;
    bool use_frontal_ = true;
    cv::Rect cam_button_;
    cv::Ptr<cv::FaceDetectorYN> detector_;
};
