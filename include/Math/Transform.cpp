#include "Transform.hpp"

namespace raytracer {
    Transform::Transform() : m(Matrix::identity()), mInv(Matrix::identity()) {}
    Transform::Transform(const Matrix& m) : m(m), mInv(m.inverse()) {}
    Transform::Transform(const Matrix& m, const Matrix& mInv) : m(m), mInv(mInv) {}

    const Matrix& Transform::getMatrix() const {
        return m; 
    }
    const Matrix& Transform::getInverseMatrix() const { 
        return mInv; 
    }

    Transform Transform::inverse() const { 
        return Transform(mInv, m); 
    
    }

    Point3 Transform::operator()(const Point3& p) const { 
        return m.applyPoint(p); 
    }
    Vector3 Transform::operator()(const Vector3& v) const { 
        return m.applyVector(v); 
    }

    Ray Transform::operator()(const Ray& r) const { 
        return m.applyRay(r); 
    }

    Bounds3 Transform::operator()(const Bounds3& b) const {
        const Transform& t = *this;
        Point3 p0 = t(Point3(b.min().getX(), b.min().getY(), b.min().getZ()));
        double minX = p0.getX(), minY = p0.getY(), minZ = p0.getZ();
        double maxX = p0.getX(), maxY = p0.getY(), maxZ = p0.getZ();

        for (int i = 1; i < 8; ++i) {
            double x = (i & 1) ? b.max().getX() : b.min().getX();
            double y = (i & 2) ? b.max().getY() : b.min().getY();
            double z = (i & 4) ? b.max().getZ() : b.min().getZ();
            Point3 p = t(Point3(x, y, z));
            minX = std::min(minX, p.getX());
            minY = std::min(minY, p.getY());
            minZ = std::min(minZ, p.getZ());
            maxX = std::max(maxX, p.getX());
            maxY = std::max(maxY, p.getY());
            maxZ = std::max(maxZ, p.getZ());
        }
        return Bounds3(Point3(minX, minY, minZ), Point3(maxX, maxY, maxZ));
    }

    Vector3 Transform::applyNormal(const Vector3& n) const { 
        return mInv.transpose().applyNormal(n); 
    }

    Transform Transform::operator*(const Transform& t2) const {
        return Transform(m * t2.m, t2.mInv * mInv);
    }
}