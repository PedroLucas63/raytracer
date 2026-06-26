#pragma once

#include "Vector3.hpp"
#include "Point3.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"
#include "Math/Tensor/Tensor.hpp"

namespace raytracer {

    class Transform{
        private:
            Tensor<double> m, mInv;
            
        public:
            Transform();
            Transform(const Tensor<double>& m);
            Transform(const Tensor<double>& m, const Tensor<double>& mInv);

            const Tensor<double>& getMatrix() const;
            const Tensor<double>& getInverseMatrix() const;
            Transform inverse() const;
            
            Point3  operator()(const Point3& p, bool inverse = false) const;
            Vector3 operator()(const Vector3& v, bool inverse = false) const;
            Ray     operator()(const Ray& r, bool inverse = false) const;
            Bounds3 operator()(const Bounds3& b, bool inverse = false) const;
            Vector3 applyNormal(const Vector3& n, bool inverse = false) const;
            
            Transform operator*(const Transform& t2) const;
            Transform dot(const Transform& t2, bool inverse = false) const;
    };
}