#pragma once

class Camera {
  public:
    void setViewport(int width, int height) {
        width_ = width;
        height_ = height;
    }
    int width() const {
        return width_;
    }
    int height() const {
        return height_;
    }
    void setInternalWidth(float internal_width) {
        internal_width_ = internal_width;
    }
    float internalWidth() const {
        return internal_width_;
    }
    float internalHeight() const {
        return internal_width_ * aspect();
    }
    float aspect() const {
        return static_cast<float>(height_) / width_;
    }
    float xLeft() const {
        return -internal_width_ / 2.f;
    }
    float yBottom() const {
        return -aspect() * internal_width_ / 2.f;
    }
    float xRight() const {
        return internal_width_ / 2.f;
    }
    float yTop() const {
        return aspect() * internal_width_ / 2.f;
    }

  private:
    int width_;
    int height_;
    float internal_width_;
};
