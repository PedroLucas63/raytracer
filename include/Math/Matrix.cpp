#include "Math/Matrix.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <stdexcept>

namespace raytracer {


Matrix4x4::Matrix4x4() {
    // identity
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
}

Matrix4x4::Matrix4x4(const float mat[4][4]) {
    std::memcpy(m, mat, sizeof(m));
}

Matrix4x4::Matrix4x4(
    float t00, float t01, float t02, float t03,
    float t10, float t11, float t12, float t13,
    float t20, float t21, float t22, float t23,
    float t30, float t31, float t32, float t33)
{
    m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
    m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
    m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
    m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
}


Matrix4x4 Matrix4x4::identity() {
    return Matrix4x4{}; // default constructor is identity
}


float Matrix4x4::operator()(int row, int col) const {
    if (row < 0 || row >= 4 || col < 0 || col >= 4)
        throw std::out_of_range("Matrix4x4: index out of range");
    return m[row][col];
}

float& Matrix4x4::operator()(int row, int col) {
    if (row < 0 || row >= 4 || col < 0 || col >= 4)
        throw std::out_of_range("Matrix4x4: index out of range");
    return m[row][col];
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const {
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += m[i][k] * rhs.m[k][j];
            }
        }
    }
    return result;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) {
    *this = *this * other;
    return *this;
}

bool Matrix4x4::operator==(const Matrix4x4& other) const {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (m[i][j] != other.m[i][j]) return false;
    return true;
}

bool Matrix4x4::operator!=(const Matrix4x4& other) const {
    return !(*this == other);
}


Matrix4x4 Matrix4x4::transpose() const {
    return Matrix4x4{
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    };
}

// Inverse — Gauss-Jordan elimination with full pivoting on 4×4
Matrix4x4 Matrix4x4::inverse() const {

}

// Apply to geometric primitives

Point3 Matrix4x4::applyPoint(const Point3& p) const {
    float x = static_cast<float>(p.getX());
    float y = static_cast<float>(p.getY());
    float z = static_cast<float>(p.getZ());

    float xp = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
    float yp = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
    float zp = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
    float wp  = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3];

    if (wp == 1.0f) return Point3(xp, yp, zp);
    return Point3(xp / wp, yp / wp, zp / wp);
}

Vector3 Matrix4x4::applyVector(const Vector3& v) const {
    float x = static_cast<float>(v.getX());
    float y = static_cast<float>(v.getY());
    float z = static_cast<float>(v.getZ());

    // Vectors are not translated (w = 0)
    return Vector3(
        m[0][0]*x + m[0][1]*y + m[0][2]*z,
        m[1][0]*x + m[1][1]*y + m[1][2]*z,
        m[2][0]*x + m[2][1]*y + m[2][2]*z
    );
}

Vector3 Matrix4x4::applyNormal(const Vector3& n) const {
    float x = static_cast<float>(n.getX());
    float y = static_cast<float>(n.getY());
    float z = static_cast<float>(n.getZ());

    // Apply transpose rows of this become columns
    return Vector3(
        m[0][0]*x + m[1][0]*y + m[2][0]*z,
        m[0][1]*x + m[1][1]*y + m[2][1]*z,
        m[0][2]*x + m[1][2]*y + m[2][2]*z
    );
}

Ray Matrix4x4::applyRay(const Ray& r) const {
    return Ray(applyPoint(r.origin), applyVector(r.direction), r.t_min, r.t_max);
}

std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat) {
    os << std::fixed << std::setprecision(4);
    for (int i = 0; i < 4; ++i) {
        os << "[ ";
        for (int j = 0; j < 4; ++j)
            os << std::setw(10) << mat.m[i][j] << " ";
        os << "]\n";
    }
    return os;
}

Matrix4x4 mat4_identity() {
    return Matrix4x4::identity();
}

Matrix4x4 mat4_translation(float tx, float ty, float tz) {
    return Matrix4x4{
        1, 0, 0, tx,
        0, 1, 0, ty,
        0, 0, 1, tz,
        0, 0, 0,  1
    };
}

Matrix4x4 mat4_scale(float sx, float sy, float sz) {
    return Matrix4x4{
        sx,  0,  0, 0,
         0, sy,  0, 0,
         0,  0, sz, 0,
         0,  0,  0, 1
    };
}

Matrix4x4 mat4_rotation(const Vector3& axis, float angleDeg) {
    const float rad = angleDeg * static_cast<float>(M_PI) / 180.0f;
    const float c   = std::cos(rad);
    const float s   = std::sin(rad);
    const float t   = 1.0f - c;

    // Normalize axis
    const float len = static_cast<float>(axis.length());
    if (len < 1e-6f)
        throw std::invalid_argument("mat4_rotation: axis vector has zero length");

    const float ax = static_cast<float>(axis.getX()) / len;
    const float ay = static_cast<float>(axis.getY()) / len;
    const float az = static_cast<float>(axis.getZ()) / len;

    // Rodrigues' rotation formula (row-major)
    return Matrix4x4{
        t*ax*ax + c,    t*ax*ay - s*az, t*ax*az + s*ay, 0,
        t*ax*ay + s*az, t*ay*ay + c,    t*ay*az - s*ax, 0,
        t*ax*az - s*ay, t*ay*az + s*ax, t*az*az + c,    0,
        0,              0,              0,              1
    };
}

Matrix4x4 mat4_compose(const Matrix4x4& parent, const Matrix4x4& child) {
    return parent * child;
}

} // namespace raytracer
