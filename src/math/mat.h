#pragma once

#include "common.h"
#include "vector.h"

namespace Math {
class Mat {
  public:
    Mat(Vector v1, Vector v2, Vector v3);

    Mat(float a00 = 1,
        float a01 = 0,
        float a02 = 0,
        float a10 = 0,
        float a11 = 1,
        float a12 = 0,
        float a20 = 0,
        float a21 = 0,
        float a22 = 1);

    const float el(int i, int j) const {
        return ar_[i * 3 + j];
    }

    float& el(int i, int j) {
        return ar_[i * 3 + j];
    }

    float det() const;

    void scale(float val);

    Mat inverse() const;

    bool operator==(const Mat& m) const;

    Mat operator*(const Mat& m) const;

    Mat operator+(const Mat& m) const;

    Mat operator*(float f) const;

    Vector operator*(const Vector& v) const;

    Mat rotate(Vector v, float angle) const;

  private:
    float ar_[9];
};

inline Mat operator*(float f, const Mat& m) {
    return m * f;
}
}  // namespace Math
