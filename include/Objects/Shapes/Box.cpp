#include "Objects/Shapes/Box.hpp"
#include "Math/Vector3.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace raytracer {

    const Point3 Box::_first = Point3(-0.5, -0.5, -0.5);
    const Point3 Box::_second = Point3(0.5, 0.5, 0.5);

    Vector3 Box::computeNormal(const Ray& ray, float tHit) const {
        const double eps = 1e-4;
        Point3 p = ray(tHit);

        if (std::abs(p.getX() - _first.getX()) < eps) return Vector3(-1,0,0);
        if (std::abs(p.getX() - _second.getX()) < eps) return Vector3(1,0,0);

        if (std::abs(p.getY() - _first.getY()) < eps) return Vector3(0,-1,0);
        if (std::abs(p.getY() - _second.getY()) < eps) return Vector3(0,1,0);

        if (std::abs(p.getZ() - _first.getZ()) < eps) return Vector3(0,0,-1);
        return {0,0,1};
    }

    bool Box::intersect(const Ray& ray, const Transform& objToWorld) const{
        Bounds3 bounds = getBounds(objToWorld);

        float tMin, tMax;
        auto localRay = objToWorld(ray, true);
        if (!bounds.intersect(localRay, tMin, tMax)) {
            return false;
        }
            
        if (tMin > tMax || tMax < localRay.t_min || tMin > localRay.t_max) return false;
        
        float tHit = tMin;
        if (tHit < localRay.t_min) {
            tHit = tMax;
            if (tHit < localRay.t_min) return false;
        }
        
        return true;
    }

    bool Box::intersectWithSurfel(const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf) const { 
        Bounds3 bounds = getBounds(objToWorld);

        float tMin, tMax;
        auto localRay = objToWorld(ray, true);
        if (!bounds.intersect(localRay, tMin, tMax)) {
            return false;
        }
        
        if (tMin > tMax || tMax < localRay.t_min || tMin > localRay.t_max) return false;
        
        float th = tMin;
        if (th < localRay.t_min) {
            th = tMax;
            if (th < localRay.t_min) return false;
        }

        if (tHit) *tHit = th;
        
        if (sf) {
            sf->t = th;
            sf->point = localRay(th);
            sf->viewDir = -localRay.direction; 
            sf->normal = computeNormal(localRay, th);
        }

        return true;
    }

    Bounds3 Box::getBounds(const Transform& objToWorld) const {
        auto p0 = objToWorld(_first);
        auto p1 = objToWorld(_second);
        return Bounds3::fromPoints(p0, p1);
    }
};