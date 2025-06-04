
#include "vector.h"

#include <cmath>

namespace Math {
Vector::Vector(float x, float y, float z) {
    vec_[0] = x;
    vec_[1] = y;
    vec_[2] = z;
}

Vector Vector::operator-(const Vector& v) const {
    return Vector(vec_[0] - v[0], vec_[1] - v[1], vec_[2] - v[2]);
}
bool Vector::operator==(const Vector& v) const {
    return fabs(dot(*this, v)) < FLOAT_EPS;
}
Vector Vector::operator-() const {
    return Vector(-vec_[0], -vec_[1], -vec_[2]);
}
Vector Vector::operator+(const Vector& v) const {
    return Vector(vec_[0] + v[0], vec_[1] + v[1], vec_[2] + v[2]);
}
Vector Vector::operator+=(const Vector& v) {
    vec_[0] += v[0];
    vec_[1] += v[1];
    vec_[2] += v[2];
    return *this;
}

Vector Vector::operator*(float f) const {
    Vector res = *this;
    res[0] = vec_[0] * f;
    res[1] = vec_[1] * f;
    res[2] = vec_[2] * f;
    return res;
}

Vector Vector::normalized() const {
    float d = 1 / len();
    Vector res = *this;
    return res * d;
}

Vector Vector::ortogonal(const Vector& v) const {
    return *this - v * (dot(*this, v) / v.len());
}

float Vector::len() const {
    return sqrtf(vec_[0] * vec_[0] + vec_[1] * vec_[1] + vec_[2] * vec_[2]);
}

Vector cross(const Vector& v1, const Vector& v2) {
    Vector r;
    r[0] = v1[1] * v2[2] - v1[2] * v2[1];
    r[1] = v1[2] * v2[0] - v1[0] * v2[2];
    r[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return r;
}

float dot(const Vector& v1, const Vector& v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
Vector centerMass(const VArray& p) {
    Vector cm;
    for (VArray::const_iterator it = p.begin(); it != p.end(); it++) {
        cm += *it;
    }
    cm = cm * (1. / p.size());
    return cm;
}
void centralize(VArray& p) {
    Vector cm = centerMass(p);
    for (VArray::iterator it = p.begin(); it != p.end(); it++) {
        *it = *it - cm;
    }
}
void scale(VArray& p, float scale) {
    for (VArray::iterator it = p.begin(); it != p.end(); it++) {
        *it = scale * (*it);
    }
}
float maxRadius(VArray& p) {
    float r = 0;
    for (VArray::iterator it = p.begin(); it != p.end(); it++) {
        float len = it->len();
        if (len > r) r = len;
    }
    return r;
}
}  // namespace Math
