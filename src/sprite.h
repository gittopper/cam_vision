#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>
#pragma pack(push, 1)

struct Color {
    Color() : r(0), g(0), b(0), a(0) {}
    Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) :
        r(r), g(g), b(b), a(a) {}
    Color(std::uint32_t c) {
        setColor(c);
    }
    void setColor(std::uint32_t c) {
        a = c % 256;
        b = (c >> 8) % 256;
        g = (c >> 16) % 256;
        r = (c >> 24) % 256;
    }
    void blend(const Color& color) {
        if (a == 255) {
            return;
        }
        auto alpha = float(a) / (a + color.a);
        r = r * alpha + color.r * (1 - alpha);
        g = g * alpha + color.g * (1 - alpha);
        b = b * alpha + color.b * (1 - alpha);
        a = std::max(a, color.a);
    }
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

#pragma pack(pop)

class Sprite {
  public:
    enum Type { RGBA, RGB };
    Sprite(std::size_t width, std::size_t height, const Color& color) :
        width_(width),
        height_(height),
        gl_width_(width),
        gl_height_(height),
        type_(RGBA),
        data_(width * height * 4) {
        auto* pixel = reinterpret_cast<Color*>(data_.data());
        for (auto i = 0UL; i < width * height; ++i) {
            *(pixel++) = color;
        }
    }
    Sprite(std::size_t width,
           std::size_t height,
           std::size_t gl_width,
           std::size_t gl_height,
           Type type,
           std::vector<std::uint8_t>&& data) :
        width_(width),
        height_(height),
        gl_width_(gl_width),
        gl_height_(gl_height),
        type_(type),
        data_(std::move(data)) {}
    std::size_t width() const {
        return width_;
    }
    std::size_t height() const {
        return height_;
    }
    std::size_t glWidth() const {
        return gl_width_;
    }
    std::size_t glHeight() const {
        return gl_height_;
    }
    void copy(const Sprite& other) {
        assert(glWidth() == other.glWidth());
        assert(glHeight() == other.glHeight());
        assert(type() == other.type());
        assert(type() == RGBA);
        std::memcpy(data_.data(), other.data_.data(),
                    glWidth() * glHeight() * 4);
    }
    const std::uint8_t* data() const {
        return data_.data();
    }
    std::uint8_t* data() {
        return data_.data();
    }
    Type type() const {
        return type_;
    }
    Color getPixel(std::size_t x, std::size_t y) const {
        if (x >= glWidth() || y >= glHeight()) {
            return {};
        }
        auto shift = (y * glWidth() + x) * (type_ == RGBA ? 4 : 3);
        return *reinterpret_cast<const Color*>(&data_[shift]);
    }
    void setPixel(std::size_t x, std::size_t y, const Color& color) {
        if (x >= glWidth() || y >= glHeight()) {
            return;
        }
        auto shift = (y * glWidth() + x) * (type_ == RGBA ? 4 : 3);
        if (type_ == RGBA) {
            *reinterpret_cast<Color*>((data_.data() + shift)) = color;
        } else {
            data_[shift] = color.r;
            data_[shift + 1] = color.g;
            data_[shift + 2] = color.b;
        }
    }
    void drawRect(std::size_t x,
                  std::size_t y,
                  std::size_t width,
                  std::size_t height,
                  const Color& color) {
        for (auto i = x; i < x + width; ++i) {
            for (auto j = y; j < y + height; ++j) {
                setPixel(i, j, color);
            }
        }
    }

  private:
    std::size_t width_;
    std::size_t height_;
    std::size_t gl_width_;
    std::size_t gl_height_;
    Type type_;
    std::vector<std::uint8_t> data_;
};
