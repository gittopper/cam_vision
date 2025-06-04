
#include <math/mat44.h>

#include <math.h>

namespace Math {

// http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
void SetOrtho(Mat44& m,
              float left,
              float right,
              float bottom,
              float top,
              float near,
              float far) {
    const float tx = -(right + left) / (right - left);
    const float ty = -(top + bottom) / (top - bottom);
    const float tz = -(far + near) / (far - near);

    m.m_[0] = 2.0f / (right - left);
    m.m_[1] = 0;
    m.m_[2] = 0;
    m.m_[3] = tx;

    m.m_[4] = 0;
    m.m_[5] = 2.0f / (top - bottom);
    m.m_[6] = 0;
    m.m_[7] = ty;

    m.m_[8] = 0;
    m.m_[9] = 0;
    m.m_[10] = -2.0 / (far - near);
    m.m_[11] = tz;

    m.m_[12] = 0;
    m.m_[13] = 0;
    m.m_[14] = 0;
    m.m_[15] = 1;
}

void SetRotY(Mat44& m, float angle) {
    const float c = cosf(angle);
    const float s = sinf(angle);

    m.m_[0] = c;
    m.m_[1] = 0;
    m.m_[2] = -s;
    m.m_[3] = 0;

    m.m_[4] = 0;
    m.m_[5] = 1;
    m.m_[6] = 0;
    m.m_[7] = 0;

    m.m_[8] = s;
    m.m_[9] = 0;
    m.m_[10] = c;
    m.m_[11] = 0;

    m.m_[12] = 0;
    m.m_[13] = 0;
    m.m_[14] = 0;
    m.m_[15] = 1;
}

void SetRotZ(Mat44& m, float angle) {
    const float c = cosf(angle);
    const float s = sinf(angle);

    m.m_[0] = c;
    m.m_[1] = -s;
    m.m_[2] = 0;
    m.m_[3] = 0;

    m.m_[4] = s;
    m.m_[5] = c;
    m.m_[6] = 0;
    m.m_[7] = 0;

    m.m_[8] = 0;
    m.m_[9] = 0;
    m.m_[10] = 1;
    m.m_[11] = 0;

    m.m_[12] = 0;
    m.m_[13] = 0;
    m.m_[14] = 0;
    m.m_[15] = 1;
}

void Mul(Mat44& m, const Mat44& a, const Mat44& b) {
    m.m_[0] = a.m_[0] * b.m_[0] + a.m_[1] * b.m_[4] + a.m_[2] * b.m_[8] +
              a.m_[3] * b.m_[12];
    m.m_[1] = a.m_[0] * b.m_[1] + a.m_[1] * b.m_[5] + a.m_[2] * b.m_[9] +
              a.m_[3] * b.m_[13];
    m.m_[2] = a.m_[0] * b.m_[2] + a.m_[1] * b.m_[6] + a.m_[2] * b.m_[10] +
              a.m_[3] * b.m_[14];
    m.m_[3] = a.m_[0] * b.m_[3] + a.m_[1] * b.m_[7] + a.m_[2] * b.m_[11] +
              a.m_[3] * b.m_[15];

    m.m_[4] = a.m_[4] * b.m_[0] + a.m_[5] * b.m_[4] + a.m_[6] * b.m_[8] +
              a.m_[7] * b.m_[12];
    m.m_[5] = a.m_[4] * b.m_[1] + a.m_[5] * b.m_[5] + a.m_[6] * b.m_[9] +
              a.m_[7] * b.m_[13];
    m.m_[6] = a.m_[4] * b.m_[2] + a.m_[5] * b.m_[6] + a.m_[6] * b.m_[10] +
              a.m_[7] * b.m_[14];
    m.m_[7] = a.m_[4] * b.m_[3] + a.m_[5] * b.m_[7] + a.m_[6] * b.m_[11] +
              a.m_[7] * b.m_[15];

    m.m_[8] = a.m_[8] * b.m_[0] + a.m_[9] * b.m_[4] + a.m_[10] * b.m_[8] +
              a.m_[11] * b.m_[12];
    m.m_[9] = a.m_[8] * b.m_[1] + a.m_[9] * b.m_[5] + a.m_[10] * b.m_[9] +
              a.m_[11] * b.m_[13];
    m.m_[10] = a.m_[8] * b.m_[2] + a.m_[9] * b.m_[6] + a.m_[10] * b.m_[10] +
               a.m_[11] * b.m_[14];
    m.m_[11] = a.m_[8] * b.m_[3] + a.m_[9] * b.m_[7] + a.m_[10] * b.m_[11] +
               a.m_[11] * b.m_[15];

    m.m_[12] = a.m_[12] * b.m_[0] + a.m_[13] * b.m_[4] + a.m_[14] * b.m_[8] +
               a.m_[15] * b.m_[12];
    m.m_[13] = a.m_[12] * b.m_[1] + a.m_[13] * b.m_[5] + a.m_[14] * b.m_[9] +
               a.m_[15] * b.m_[13];
    m.m_[14] = a.m_[12] * b.m_[2] + a.m_[13] * b.m_[6] + a.m_[14] * b.m_[10] +
               a.m_[15] * b.m_[14];
    m.m_[15] = a.m_[12] * b.m_[3] + a.m_[13] * b.m_[7] + a.m_[14] * b.m_[11] +
               a.m_[15] * b.m_[15];
}

void convertTo44(Mat44& m, const Mat& m33, const Vector& v) {
    m.m_[0] = m33.el(0, 0);
    m.m_[4] = m33.el(0, 1);
    m.m_[8] = m33.el(0, 2);

    m.m_[1] = m33.el(1, 0);
    m.m_[5] = m33.el(1, 1);
    m.m_[9] = m33.el(1, 2);

    m.m_[2] = m33.el(2, 0);
    m.m_[6] = m33.el(2, 1);
    m.m_[10] = m33.el(2, 2);

    m.m_[12] = v[0];
    m.m_[13] = v[1];
    m.m_[14] = v[2];

    m.m_[15] = 1;

    m.m_[3] = 0;
    m.m_[7] = 0;
    m.m_[11] = 0;
}

}  // namespace Math
