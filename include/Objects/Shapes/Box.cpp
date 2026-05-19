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

        _first = params.retrieve<Point3>("first_point");
        _second = params.retrieve<Point3>("second_point");
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

        double minX = std::min(_first.getX(), _second.getX());
        double maxX = std::max(_first.getX(), _second.getX());
        double minY = std::min(_first.getY(), _second.getY());
        double maxY = std::max(_first.getY(), _second.getY());
        double minZ = std::min(_first.getZ(), _second.getZ());
        double maxZ = std::max(_first.getZ(), _second.getZ());

        if (!checkAxis(ray.origin.getX(), ray.direction.getX(), minX, maxX)) return {-1.0f, -1.0f};
        if (!checkAxis(ray.origin.getY(), ray.direction.getY(), minY, maxY)) return {-1.0f, -1.0f};
        if (!checkAxis(ray.origin.getZ(), ray.direction.getZ(), minZ, maxZ)) return {-1.0f, -1.0f};

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
            double minX = std::min(_first.getX(), _second.getX());
            double maxX = std::max(_first.getX(), _second.getX());
            double minY = std::min(_first.getY(), _second.getY());
            double maxY = std::max(_first.getY(), _second.getY());
            double minZ = std::min(_first.getZ(), _second.getZ());
            double maxZ = std::max(_first.getZ(), _second.getZ());
            
            Vector3 normal(0, 0, 0);
            if (std::abs(sf->point.getX() - minX) < eps) normal = Vector3(-1, 0, 0);
            else if (std::abs(sf->point.getX() - maxX) < eps) normal = Vector3(1, 0, 0);
            else if (std::abs(sf->point.getY() - minY) < eps) normal = Vector3(0, -1, 0);
            else if (std::abs(sf->point.getY() - maxY) < eps) normal = Vector3(0, 1, 0);
            else if (std::abs(sf->point.getZ() - minZ) < eps) normal = Vector3(0, 0, -1);
            else if (std::abs(sf->point.getZ() - maxZ) < eps) normal = Vector3(0, 0, 1);
            
            sf->normal = normal;
        }

        return true;
    }

    Bounds3 Box::getBounds() const {
        double minX = std::min(_first.getX(), _second.getX());
        double maxX = std::max(_first.getX(), _second.getX());
        double minY = std::min(_first.getY(), _second.getY());
        double maxY = std::max(_first.getY(), _second.getY());
        double minZ = std::min(_first.getZ(), _second.getZ());
        double maxZ = std::max(_first.getZ(), _second.getZ());
        
        return Bounds3(Point3(minX, minY, minZ), Point3(maxX, maxY, maxZ));
    }

};