#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using Clock = std::chrono::system_clock;

class Timer {
  public:
    Timer() {
        start();
    }

    double totalTime() const {
        return total_time_;
    }

    int getN() const {
        return n_;
    }

    double getAvgTime() const {
        return n_ > 0 ? total_time_ / n_ : time();
    }

    void start() {
        begin_time_ = now();
    }
    double stop() {
        float cur_time = now();
        elapsed_time_ = cur_time - begin_time_;
        ++n_;
        total_time_ += elapsed_time_;
        return elapsed_time_;
    }

    double time() const {
        double cur_time = now();
        auto elapsed_time = cur_time - begin_time_;
        return elapsed_time;
    }
    void reset() {
        start();
    }
    static std::string asString(float t) {
        int ms = int(t * 1000) % 1000;
        auto time_int = static_cast<int>(t);
        int s = time_int % 60;
        int m = time_int / 60;
        int h = time_int / 3600;
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << h << ":";
        ss << std::setw(2) << std::setfill('0') << m << ":";
        ss << std::setw(2) << std::setfill('0') << s << "." << ms;
        return ss.str();
    }

    std::string elapsedAsString() const {
        return asString(time());
    }

    ~Timer() {}

    // in seconds
    static double now() {
        return static_cast<double>(clock()) / CLOCKS_PER_SEC;
    }

  private:
    double begin_time_ = 0;
    int n_ = 0;
    double total_time_ = 0;
    double elapsed_time_ = 0;
};

inline std::string getTimestamp() {
    auto time = Clock::now();
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            time.time_since_epoch())
                            .count();
    auto t = std::chrono::system_clock::to_time_t(time);
    struct tm* tm = localtime(&t);
    char buf[100] = {};
    strftime(buf, 100, "%Y.%m.%d %H:%M:%S.", tm);
    std::stringstream ss;
    ss << buf;
    ss << std::setw(3) << std::setfill('0') << (timestamp_ms) % 1000;
    return ss.str();
}
