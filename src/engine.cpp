#include <engine.h>

#include <opencv2/imgproc.hpp>

Engine::Engine() {}

void Engine::tap(int x, int y) {
    if (x >= cam_button_.x && x <= cam_button_.x + cam_button_.width &&
            y >= cam_button_.y && y <= cam_button_.y + cam_button_.height)
    {
        std::lock_guard<std::mutex> lock(m_);
        use_frontal_ = !use_frontal_;
        web_cam_->close();
        web_cam_ = cam_builder_->open(use_frontal_);
    }
}

void Engine::setup(int w, int h, ResourceLoader* loader) {
    camera_.setViewport(w, h);
    camera_.setInternalWidth(10);
    auto wb = std::min(w, h) * 0.05;
    cam_button_ = cv::Rect(wb, wb, wb, wb);
    renderer_.setCamera(&camera_);
    auto yunet = loader->readFileU("face_detection_yunet_2023mar.onnx");
    detector_ = cv::FaceDetectorYN::create("onnx", yunet, {}, cv::Size(w, h));
    web_cam_ = cam_builder_->open(use_frontal_);
    cam_thread_ = std::thread([this](){
        while(do_capture_) {
            cv::Mat frame;
            {
                std::lock_guard<std::mutex> lock(m_);
                frame = web_cam_->getImage();
            }
            if(!frame.empty()) {
                cv::Mat input;
                cv::cvtColor(frame, input, cv::COLOR_RGBA2BGR);
                cv::Mat faces;
                detector_->setInputSize(input.size());
                detector_->detect(input, faces);

                const cv::Scalar color(255, 0, 0, 255);
                auto draw_pimp = [&color, &frame](float x, float y){
                    cv::circle(frame, cv::Point(x, y), 2, color, -1);
                };
                for(auto iface = 0; iface < faces.rows; ++iface) {
                    auto x = faces.at<float>(iface, 0);
                    auto y = faces.at<float>(iface, 1);
                    auto w = faces.at<float>(iface, 2);
                    auto h = faces.at<float>(iface, 3);
                    cv::rectangle(frame, cv::Rect(x, y, w, h), color, 2);
                    auto eye_right_x = faces.at<float>(iface, 4);
                    auto eye_right_y = faces.at<float>(iface, 5);
                    draw_pimp(eye_right_x, eye_right_y);
                    auto eye_left_x = faces.at<float>(iface, 6);
                    auto eye_left_y = faces.at<float>(iface, 7);
                    draw_pimp(eye_left_x, eye_left_y);
                    auto nose_tip_x = faces.at<float>(iface, 8);
                    auto nose_tip_y = faces.at<float>(iface, 9);
                    draw_pimp(nose_tip_x, nose_tip_y);
                    auto mouth_right_x = faces.at<float>(iface, 10);
                    auto mouth_right_y = faces.at<float>(iface, 11);
                    draw_pimp(mouth_right_x, mouth_right_y);
                    auto mouth_left_x = faces.at<float>(iface, 12);
                    auto mouth_left_y = faces.at<float>(iface, 13);
                    draw_pimp(mouth_left_x, mouth_left_y);
                    auto score = faces.at<float>(iface, 14);
                    auto text = std::to_string(score);
                    auto font_size = 2;
                    cv::rectangle(frame, cv::Point{int(x), int(y) - font_size * 12},
                            cv::Point{int(x) + int(text.length()) * font_size * 10, int(y)},color,-1);
                    cv::putText(frame, text, cv::Point(x, y), cv::FONT_HERSHEY_PLAIN, font_size, cv::Scalar(255, 255, 255), 2);
                }
                auto w = camera_.width();
                auto h = camera_.height();
                auto ratio = float(w) / h;
                auto cam_ratio = float(frame.cols) / frame.rows;
                auto fw = w - 1;
                auto fh = h - 1;
                if(cam_ratio > ratio) {
                    fh = int(w / cam_ratio);
                } else {
                    fw = int(h * cam_ratio);
                }
                cv::Mat resized;
                cv::resize(frame, resized, cv::Size(fw, fh));
                auto x_shift = (w - fw) / 2;
                auto y_shift = (h - fh) / 2;
                std::lock_guard<std::mutex> lock(m_);
                cam_image_ = std::make_shared<Sprite>(w, h, Color(0, 0, 0, 255));
                cv::Mat wrapper(h, w, CV_8UC4, cam_image_->data());
                cv::Mat roi(wrapper, cv::Rect(x_shift, y_shift, fw, fh));
                resized.copyTo(roi);
                cv::imwrite("wrapper.png", wrapper);
                cv::rectangle(wrapper, cam_button_, cv::Scalar(255), -1);

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
