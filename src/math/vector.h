#pragma once

#include <cfloat>
#include <vector>

#include "common.h"

using namespace std;

namespace Math {
class Vector {
  public:
    Vector(float x = 0, float y = 0, float z = 0);

    const float& operator[](int i) const {
        return vec_[i];
    }

    Vector& operator=(const Vector& v) {
        vec_[0] = v.vec_[0];
        vec_[1] = v.vec_[1];
        vec_[2] = v.vec_[2];
        return *this;
    }

    float& operator[](int i) {
        return vec_[i];
    }

    Vector operator*(float f) const;

    Vector operator-(const Vector& v) const;

    bool operator==(const Vector& v) const;

    Vector operator-() const;

    Vector operator+(const Vector& v) const;

    bool operator<(const Vector& v) const {
        return vec_[0] < v[0] && vec_[1] < v[1] && vec_[2] < v[2];
    }

    bool operator>(const Vector& v) const {
        return vec_[0] > v[0] && vec_[1] > v[1] && vec_[2] > v[2];
    }

    bool operator<=(const Vector& v) const {
        return vec_[0] <= v[0] && vec_[1] <= v[1] && vec_[2] <= v[2];
    }

    bool operator>=(const Vector& v) const {
        return vec_[0] >= v[0] && vec_[1] >= v[1] && vec_[2] >= v[2];
    }

    Vector operator+=(const Vector& v);

    Vector normalized() const;

    // ortogonal part to another vector
    Vector ortogonal(const Vector&) const;

    float len() const;

  private:
    float vec_[3];
};

inline Vector operator*(float f, Vector& v) {
    return v * f;
}

Vector cross(const Vector& v1, const Vector& v2);

float dot(const Vector& v1, const Vector& v2);

typedef vector<Vector> VArray;

Vector centerMass(const VArray& p);
void centralize(VArray& p);

void scale(VArray& p, float scale);
float maxRadius(VArray& p);

#define MAX_VECTOR Vector(FLT_MAX, FLT_MAX, FLT_MAX)
#define MIN_VECTOR Vector(FLT_MIN, FLT_MIN, FLT_MIN)
}  // namespace Math
