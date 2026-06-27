#pragma once

#include "Matrix.hpp"
#include "Vector3.hpp"
#include "Point3.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"

namespace raytracer {

    class Transform {
        private:
            Matrix m, mInv;
            bool _isIdentity;
        public:
            Transform();
            Transform(const Matrix& m);
            Transform(const Matrix& m, const Matrix& mInv);

            const Matrix& getMatrix() const;
            const Matrix& getInverseMatrix() const;
            Transform inverse() const;
            bool isIdentity() const;
            
            Point3  operator()(const Point3& p, bool applyInverse = false) const;
            Vector3 operator()(const Vector3& v, bool applyInverse = false) const;
            Ray     operator()(const Ray& r, bool applyInverse = false) const;
            Bounds3 operator()(const Bounds3& b, bool applyInverse = false) const;
            
            Vector3 applyNormal(const Vector3& n, bool applyInverse = false) const;
            
            Transform operator*(const Transform& t2) const;
            Transform dot(const Transform& t2, bool applyInverse = false) const;
    };

    Transform translation(const Vector3& v);
    Transform scale      (const Vector3& v);
    Transform rotation   (const Vector3& axis, float angleDeg);

    // Compose: parent * child  (i.e. apply child first, then parent)
    Transform compose(const Transform& parent, const Transform& child);

}