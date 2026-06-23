#pragma once

#include "Math/Vector3.hpp"
#include "Math/Point3.hpp"
#include "Math/Ray.hpp"

#include <array>
#include <cmath>
#include <ostream>
#include <stdexcept>

namespace raytracer {

    class Matrix4x4 {
    public:
        float m[4][4];

        Matrix4x4();
        explicit Matrix4x4(const float mat[4][4]);
        Matrix4x4(
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33
        );

        float  operator()(int row, int col) const;
        float& operator()(int row, int col);
        static Matrix4x4 identity();
        Matrix4x4 operator*(const Matrix4x4& other) const;
        Matrix4x4& operator*=(const Matrix4x4& other);

        bool operator==(const Matrix4x4& other) const;
        bool operator!=(const Matrix4x4& other) const;

        // Matrix operations
        Matrix4x4 transpose() const;
        Matrix4x4 inverse()   const; 

        // Apply to geometric primitives
        Point3  applyPoint (const Point3&  p) const;
        Vector3 applyVector(const Vector3& v) const;

        // Apply the inverse-transpose to a normal (for correct shading)
        Vector3 applyNormal(const Vector3& n) const;
        Ray     applyRay   (const Ray& r)     const;

        // Debug
        friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& m);
    };


    Matrix4x4 mat4_identity();
    Matrix4x4 mat4_translation(float tx, float ty, float tz);
    Matrix4x4 mat4_scale      (float sx, float sy, float sz);
    Matrix4x4 mat4_rotation   (const Vector3& axis, float angleDeg);

    // Compose: parent * child  (i.e. apply child first, then parent)
    Matrix4x4 mat4_compose(const Matrix4x4& parent, const Matrix4x4& child);

    using Matrix = Matrix4x4;

} // namespace raytracer