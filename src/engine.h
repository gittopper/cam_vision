#pragma once

#include <rendering/glesrenderer.h>
#include <sprite.h>
#include <mutex>
#include <icamera.h>
#include <thread>

class Engine{
public:
    Engine();
    ~Engine();
    void setup(int w, int h);
    void resize(int w, int h);
    void step();
    Renderer& renderer() {
        return renderer_;
    }
    std::shared_ptr<ICamera>& webCam() {
        return web_cam_;
    }
    void pause(){}
    void resume(){}
private:
    Camera camera_;
    std::shared_ptr<ICamera> web_cam_;
    std::thread cam_thread_;
    GLESRenderer renderer_;
    std::shared_ptr<Sprite> cam_image_;
    std::mutex m_;
    bool do_capture_ = true;

};
