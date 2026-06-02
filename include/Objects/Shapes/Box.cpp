#include "Objects/Shapes/Box.hpp"
#include "Math/Vector3.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace raytracer {
    Box::Box(const ParamSet& params) : Shape(){
        if (!params.has("first_point")) {
            throw std::invalid_argument("Box requires a 'first_point' parameter");
        }
        if (!params.has("second_point")) {
            throw std::invalid_argument("Box requires a 'second_point' parameter");
        }

        Point3 a = params.retrieve<Point3>("first_point");
        Point3 b = params.retrieve<Point3>("second_point");

        _first = Point3(
            std::min(a.getX(), b.getX()),
            std::min(a.getY(), b.getY()),
            std::min(a.getZ(), b.getZ())
        );

        _second = Point3(
            std::max(a.getX(), b.getX()),
            std::max(a.getY(), b.getY()),
            std::max(a.getZ(), b.getZ())
        );
    }

    bool Box::intersect(const Ray& ray) const{
        Bounds3 bounds = getBounds();

        float tMin, tMax;
        if (!bounds.intersect(ray, tMin, tMax)) {
            return false;
        }
            
        if (tMin > tMax || tMax < ray.t_min || tMin > ray.t_max) return false;
        
        float tHit = tMin;
        if (tHit < ray.t_min) {
            tHit = tMax;
            if (tHit < ray.t_min) return false;
        }
        
        return true;
    }

    bool Box::intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const { 
        Bounds3 bounds = getBounds();

        float tMin, tMax;
        if (!bounds.intersect(ray, tMin, tMax)) {
            return false;
        }
        
        if (tMin > tMax || tMax < ray.t_min || tMin > ray.t_max) return false;
        
        float th = tMin;
        if (th < ray.t_min) {
            th = tMax;
            if (th < ray.t_min) return false;
        }

        if (tHit) *tHit = th;
        
        if (sf) {
            sf->t = th;
            sf->point = ray(th);
            sf->viewDir = Vector3(0, 0, 0) - ray.direction; 
            
            double eps = 1e-4;
            Vector3 normal(0, 0, 0);
            if (std::abs(sf->point.getX() - _first.getX()) < eps) normal = Vector3(-1, 0, 0);
            else if (std::abs(sf->point.getX() - _second.getX()) < eps) normal = Vector3(1, 0, 0);
            else if (std::abs(sf->point.getY() - _first.getY()) < eps) normal = Vector3(0, -1, 0);
            else if (std::abs(sf->point.getY() - _second.getY()) < eps) normal = Vector3(0, 1, 0);
            else if (std::abs(sf->point.getZ() - _first.getZ()) < eps) normal = Vector3(0, 0, -1);
            else if (std::abs(sf->point.getZ() - _second.getZ()) < eps) normal = Vector3(0, 0, 1);
            
            sf->normal = normal;
        }

        return true;
    }

    Bounds3 Box::getBounds() const {
        return Bounds3(_first, _second);
    }
};