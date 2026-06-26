#include "Transform.hpp"
#include "Utils/TensorUtils.hpp"

namespace raytracer {
    Transform::Transform() : m(identity()), mInv(identity()) {}
    Transform::Transform(const Tensor<double>& m) : m(m), mInv(m.inverse()) {}
    Transform::Transform(const Tensor<double>& m, const Tensor<double>& mInv) : m(m), mInv(mInv) {}

    const Tensor<double>& Transform::getMatrix() const {
        return m; 
    }
    const Tensor<double>& Transform::getInverseMatrix() const { 
        return mInv; 
    }

    Transform Transform::inverse() const { 
        return Transform(mInv, m); 
    }

    Point3 Transform::operator()(const Point3& p, bool inverse) const { 
        return inverse ? applyPoint(mInv, p) : applyPoint(m, p); 
    }
    Vector3 Transform::operator()(const Vector3& v, bool inverse) const { 
        return inverse ? applyVector(mInv, v) : applyVector(m, v); 
    }

    Ray Transform::operator()(const Ray& r, bool inverse) const { 
        return inverse ? applyRay(mInv, r) : applyRay(m, r); 
    }

    Bounds3 Transform::operator()(const Bounds3& b, bool inverse) const {
        return inverse ? applyBounds(mInv, b) : applyBounds(m, b);
    }

    Vector3 Transform::applyNormal(const Vector3& n, bool inverse) const { 
        if (inverse) {
            return applyVector(m.transpose(), n).normalize();
        }

        return applyVector(mInv.transpose(), n).normalize();
    }

    Transform Transform::operator*(const Transform& t2) const {
        return Transform(compose(m, t2.m), compose(t2.mInv, mInv));
    }

    Transform Transform::dot(const Transform& t2, bool inverse) const {
        if (inverse) {
            return Transform(compose(mInv, t2.m), compose(t2.mInv, m));
        }

        return Transform(compose(m, t2.m), compose(t2.mInv, mInv));
    }
}