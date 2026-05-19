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

    std::pair<float, float> Box::calculateIntersectPoints(const Ray& ray) const{
        float tMin = -std::numeric_limits<float>::infinity();
        float tMax = std::numeric_limits<float>::infinity();

        auto checkAxis = [&](double originCoord, double dirCoord, double minCoord, double maxCoord) {
            if (std::abs(dirCoord) < 1e-6) {
                if (originCoord < minCoord || originCoord > maxCoord) {
                    return false;
                }
            } else {
                double t0 = (minCoord - originCoord) / dirCoord;
                double t1 = (maxCoord - originCoord) / dirCoord;
                if (t0 > t1) std::swap(t0, t1);
                tMin = std::max(tMin, static_cast<float>(t0));
                tMax = std::min(tMax, static_cast<float>(t1));
                if (tMin > tMax) return false;
            }
            return true;
        };

        if (!checkAxis(ray.origin.getX(), ray.direction.getX(), _first.getX(), _second.getX())) return {-1.0f, -1.0f};
        if (!checkAxis(ray.origin.getY(), ray.direction.getY(), _first.getY(), _second.getY())) return {-1.0f, -1.0f};
        if (!checkAxis(ray.origin.getZ(), ray.direction.getZ(), _first.getZ(), _second.getZ())) return {-1.0f, -1.0f};

        return {tMin, tMax};
    }

    bool Box::intersect(const Ray& ray) const{
        auto points = calculateIntersectPoints(ray);
        float tMin = points.first;
        float tMax = points.second;
        
        if (tMin > tMax || tMax < ray.t_min || tMin > ray.t_max) return false;
        
        float tHit = tMin;
        if (tHit < ray.t_min) {
            tHit = tMax;
            if (tHit < ray.t_min) return false;
        }
        
        return true;
    }

    bool Box::intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const{
        auto points = calculateIntersectPoints(ray);
        float tMin = points.first;
        float tMax = points.second;
        
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