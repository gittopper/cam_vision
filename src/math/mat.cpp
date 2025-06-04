

#include "mat.h"

#include <cmath>

namespace Math {
Mat::Mat(Vector v1, Vector v2, Vector v3) {
    el(0, 0) = v1[0];
    el(1, 0) = v1[1];
    el(2, 0) = v1[2];

    el(0, 1) = v2[0];
    el(1, 1) = v2[1];
    el(2, 1) = v2[2];

    el(0, 2) = v3[0];
    el(1, 2) = v3[1];
    el(2, 2) = v3[2];
}
Mat::Mat(float a00,
         float a01,
         float a02,
         float a10,
         float a11,
         float a12,
         float a20,
         float a21,
         float a22) {
    el(0, 0) = a00;
    el(0, 1) = a01;
    el(0, 2) = a02;

    el(1, 0) = a10;
    el(1, 1) = a11;
    el(1, 2) = a12;

    el(2, 0) = a20;
    el(2, 1) = a21;
    el(2, 2) = a22;
}
float Mat::det() const {
    return el(0, 0) * el(1, 1) * el(2, 2) + el(1, 0) * el(2, 1) * el(0, 2) +
           el(0, 1) * el(1, 2) * el(2, 0) - el(2, 0) * el(1, 1) * el(0, 2) -
           el(1, 0) * el(0, 1) * el(2, 2) - el(2, 1) * el(1, 2) * el(0, 0);
}
void Mat::scale(float val) {
    for (int i = 0; i < 9; i++) {
        float r = ar_[i] * val;
        ar_[i] = r;
    }
}
Mat Mat::inverse() const {
    Mat inv;
    inv.el(0, 0) = (el(1, 1) * el(2, 2) - el(1, 2) * el(2, 1));
    inv.el(0, 1) = (el(0, 2) * el(2, 1) - el(0, 1) * el(2, 2));
    inv.el(0, 2) = (el(0, 1) * el(1, 2) - el(0, 2) * el(1, 1));

    inv.el(1, 0) = (el(1, 2) * el(2, 0) - el(1, 0) * el(2, 2));
    inv.el(1, 1) = (el(0, 0) * el(2, 2) - el(0, 2) * el(2, 0));
    inv.el(1, 2) = (el(0, 2) * el(1, 0) - el(0, 0) * el(1, 2));

    inv.el(2, 0) = (el(1, 0) * el(2, 1) - el(1, 1) * el(2, 0));
    inv.el(2, 1) = (el(0, 1) * el(2, 0) - el(0, 0) * el(2, 1));
    inv.el(2, 2) = (el(0, 0) * el(1, 1) - el(0, 1) * el(1, 0));
    inv.scale(1. / det());
    return inv;
}
bool Mat::operator==(const Mat& m) const {
    for (int i = 0; i < 9; i++) {
        if (ar_[i] != m.ar_[i]) return false;
    }
    return true;
}
Mat Mat::operator*(const Mat& m) const {
    Mat n;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            n.el(i, j) = el(i, 0) * m.el(0, j) + el(i, 1) * m.el(1, j) +
                         el(i, 2) * m.el(2, j);
        }
    }
    return n;
}

Mat Mat::operator+(const Mat& m) const {
    Mat n;
    for (int i = 0; i < 9; i++) {
        n.ar_[i] = ar_[i] + m.ar_[i];
    }
    return n;
}

Vector Mat::operator*(const Vector& v) const {
    Vector o;
    o[0] = el(0, 0) * v[0] + el(0, 1) * v[1] + el(0, 2) * v[2];
    o[1] = el(1, 0) * v[0] + el(1, 1) * v[1] + el(1, 2) * v[2];
    o[2] = el(2, 0) * v[0] + el(2, 1) * v[1] + el(2, 2) * v[2];
    return o;
}

Mat Mat::operator*(float f) const {
    Mat res(*this);
    for (int i = 0; i < 9; i++) {
        res.ar_[i] = res.ar_[i] * f;
    }
    return res;
}
Mat Mat::rotate(Vector v, float angle) const {
    Vector norm = v.normalized();
    Mat w(0, -norm[2], norm[1], norm[2], 0, -norm[0], -norm[1], norm[0], 0);
    // Rodrigues' Rotation Formula
    Mat res;
    res = res + w * sin(angle) + w * w * (1 - cos(angle));
    return res;
}
}  // namespace Math
