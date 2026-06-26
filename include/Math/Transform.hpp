#pragma once

#include "Matrix.hpp"
#include "Vector3.hpp"
#include "Point3.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"

namespace raytracer {

class Transform{
private:
    Matrix m, mInv;
public:
    Transform();
    Transform(const Matrix& m);
    Transform(const Matrix& m, const Matrix& mInv);

    const Matrix& getMatrix() const;
    const Matrix& getInverseMatrix() const;
    Transform inverse() const;
    
    Point3  operator()(const Point3& p) const;
    Vector3 operator()(const Vector3& v) const;
    Ray     operator()(const Ray& r) const;
    Bounds3 operator()(const Bounds3& b) const;
    
    Vector3 applyNormal(const Vector3& n) const;
    
    Transform operator*(const Transform& t2) const;
};

}