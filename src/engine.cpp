#include <engine.h>



Engine::Engine() {}

void Engine::setup(int w, int h) {
    camera_.setViewport(w, h);
    camera_.setInternalWidth(10);
    renderer_.setCamera(&camera_);
    cam_thread_ = std::thread([this](){
        while(do_capture_) {
            auto frame = web_cam_->getImage();
            if(!frame.empty()) {
                std::vector<std::uint8_t> data(4 * frame.rows * frame.cols);
                std::memcpy(data.data(), frame.data, 4 * frame.rows * frame.cols);
                std::lock_guard<std::mutex> lock(m_);
                cam_image_ = std::make_shared<Sprite>(frame.cols, frame.rows, frame.cols, frame.rows, Sprite::RGBA,
                                    std::move(data));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void Engine::resize(int w, int h) {
    camera_.setViewport(w, h);
}

Engine::~Engine() {
    if(cam_thread_.joinable()) {
        do_capture_ = false;
        cam_thread_.join();
    }
}

void Engine::step(){
    renderer_.prepareFrame();
    std::shared_ptr<Sprite> cam_image;
    {
        std::lock_guard<std::mutex> lock(m_);
        cam_image = cam_image_;
    }
    if(cam_image) {
        renderer_.drawOverlay(*cam_image);
    }
    renderer_.showFrame();
}
