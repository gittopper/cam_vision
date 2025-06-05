#include <engine.h>

#include <opencv2/imgproc.hpp>

Engine::Engine() {}

void Engine::setup(int w, int h, ResourceLoader* loader) {
    camera_.setViewport(w, h);
    camera_.setInternalWidth(10);
    renderer_.setCamera(&camera_);
    auto yunet = loader->readFileU("face_detection_yunet_2023mar.onnx");
//    net_ = cv::dnn::readNetFromONNX(yunet);
    detector_ = cv::FaceDetectorYN::create("onnx", yunet, {}, cv::Size(w, h));
    cam_thread_ = std::thread([this](){
        while(do_capture_) {
            auto frame = web_cam_->getImage();
            if(!frame.empty()) {
                cv::Mat input;
                cv::cvtColor(frame, input, cv::COLOR_RGBA2BGR);
                cv::Mat faces;
                detector_->setInputSize(input.size());
                detector_->detect(input, faces);
//                cv::Mat input_blob = cv::dnn::blobFromImage(input, 1. / 255, frame.size());
//                net_.setInput(input_blob);
//                std::vector<std::string> names = net_.getUnconnectedOutLayersNames();
//                std::vector<cv::Mat> outs;

//                net_.forward(outs, names);

//                for(const auto& out: outs) {
//                    auto r = out.rows;
//                    auto c = out.cols;
//                    auto d = out.data;
//                }
                for(auto iface = 0; iface < faces.rows; ++iface) {
                    auto x = faces.at<float>(iface, 0);
                    auto y = faces.at<float>(iface, 1);
                    auto w = faces.at<float>(iface, 2);
                    auto h = faces.at<float>(iface, 3);
                    cv::rectangle(frame, cv::Rect(x, y, w, h), cv::Scalar(255, 0, 0, 255), 2);
                }
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
