#include "Transform.hpp"

namespace raytracer {
    Transform::Transform() : m(Matrix::identity()), mInv(Matrix::identity()), _isIdentity(true) {}
    Transform::Transform(const Matrix& m) : m(m), mInv(m.inverse()), _isIdentity(m == Matrix::identity()) {}
    Transform::Transform(const Matrix& m, const Matrix& mInv) : m(m), mInv(mInv), _isIdentity(m == Matrix::identity()) {}

    const Matrix& Transform::getMatrix() const {
        return m; 
    }
    const Matrix& Transform::getInverseMatrix() const { 
        return mInv; 
    }

    Transform Transform::inverse() const { 
        if (_isIdentity) return *this;
        return Transform(mInv, m); 
    }

    bool Transform::isIdentity() const {
        return _isIdentity;
    }

    Point3 Transform::operator()(const Point3& p, bool applyInverse) const { 
        if (_isIdentity) return p;
        return applyInverse ? mInv.applyPoint(p) : m.applyPoint(p); 
    }
    Vector3 Transform::operator()(const Vector3& v, bool applyInverse) const { 
        if (_isIdentity) return v;
        return applyInverse ? mInv.applyVector(v) : m.applyVector(v); 
    }

    Ray Transform::operator()(const Ray& r, bool applyInverse) const { 
        if (_isIdentity) return r;
        const auto& M = applyInverse ? mInv : m;

        Ray out;
        out.origin = M.applyPoint(r.origin);
        out.direction = M.applyVector(r.direction);

        out.t_min = r.t_min;
        out.t_max = r.t_max;

        return out;
    }

    Bounds3 Transform::operator()(const Bounds3& b, bool applyInverse) const {
        if (_isIdentity) return b;
        const Transform& t = applyInverse ? inverse() : *this;
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

    Vector3 Transform::applyNormal(const Vector3& n, bool applyInverse) const { 
        if (_isIdentity) return n.normalize();
        auto result = applyInverse ? m.transpose().applyVector(n) : mInv.transpose().applyVector(n); 
        return result.normalize();
    }

    Transform Transform::operator*(const Transform& t2) const {
        if (_isIdentity) return t2;
        if (t2._isIdentity) return *this;
        return Transform(m * t2.m, t2.mInv * mInv);
    }

    Transform Transform::dot(const Transform& t2, bool applyInverse) const {
        if (applyInverse) {
            if (_isIdentity) return t2.inverse();
            if (t2._isIdentity) return inverse();
            return Transform(mInv * t2.mInv, t2.m * m);
        } else {
            if (_isIdentity) return t2;
            if (t2._isIdentity) return *this;
            return Transform(m * t2.m, t2.mInv * mInv);
        }
    }

    Transform translation(const Vector3& v) {
        Matrix4x4 m {
            1, 0, 0, v.getX(),
            0, 1, 0, v.getY(),
            0, 0, 1, v.getZ(),
            0, 0, 0,  1
        };

        Matrix4x4 mInv {
            1, 0, 0, -v.getX(),
            0, 1, 0, -v.getY(),
            0, 0, 1, -v.getZ(),
            0, 0, 0,   1
        };
        
        return Transform(m, mInv);
    }

    Transform scale(const Vector3& v) {
        Matrix4x4 m {
            v.getX(), 0       , 0        , 0,
            0       , v.getY(), 0        , 0,
            0       , 0       , v.getZ() , 0,
            0       , 0       , 0        , 1
        };

        Matrix4x4 mInv {
            1/v.getX(), 0         , 0         , 0,
            0         , 1/v.getY(), 0         , 0,
            0         , 0         , 1/v.getZ(), 0,
            0         , 0         , 0         , 1
        };
        
        return Transform(m, mInv);
    }

    Transform rotation(const Vector3& axis, float angleDeg) {
        const auto makeRotation = [&](float angle) {
            const float rad = angle * static_cast<float>(M_PI) / 180.0f;
            const float c   = std::cos(rad);
            const float s   = std::sin(rad);
            const float t   = 1.0f - c;

            const float len = static_cast<float>(axis.length());
            if (len < 1e-6f)
                throw std::invalid_argument("rotation: axis vector has zero length");

            const float ax = static_cast<float>(axis.getX()) / len;
            const float ay = static_cast<float>(axis.getY()) / len;
            const float az = static_cast<float>(axis.getZ()) / len;

            return Matrix4x4{
                t*ax*ax + c,    t*ax*ay - s*az, t*ax*az + s*ay, 0,
                t*ax*ay + s*az, t*ay*ay + c,    t*ay*az - s*ax, 0,
                t*ax*az - s*ay, t*ay*az + s*ax, t*az*az + c,    0,
                0,              0,              0,              1
            };
        };

        Matrix4x4 m    = makeRotation(angleDeg);
        Matrix4x4 mInv = makeRotation(-angleDeg);

        return Transform(m, mInv);
    }

    Transform compose(const Transform& parent, const Transform& child) {
        return parent * child;
    }
}