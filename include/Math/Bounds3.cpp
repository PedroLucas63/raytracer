#include "Math/Bounds3.hpp"
#include "Math/Routines.hpp"

namespace raytracer {
   Bounds3 Bounds3::fromPoints(const Point3& p1, const Point3& p2) {
      return Bounds3(raytracer::min(p1, p2), raytracer::max(p1, p2));
   }

   Bounds3 Bounds3::infinite() {
      Bounds3 bounds;
      bounds._isInfinity = true;
      return bounds;
   }

   const Point3& Bounds3::min() const {
      return _min;
   }

   const Point3& Bounds3::max() const {
      return _max;
   }

   Bounds3 Bounds3::merge(const Bounds3& bounds) const {
      if (_isInfinity || bounds._isInfinity) {
         return Bounds3::infinite();
      }
      auto p_min = raytracer::min(_min, bounds._min);
      auto p_max = raytracer::max(_max, bounds._max);

      return Bounds3(p_min, p_max);
   }

   Vector3 Bounds3::diagonal() const {
      return _max - _min;
   }

   Axis Bounds3::maxExtent() const {
      Vector3 d = diagonal();
      if (d.getX() > d.getY() && d.getX() > d.getZ()) {
         return Axis::X;
      } else if (d.getY() > d.getZ()) {
         return Axis::Y;
      } else {
         return Axis::Z;
      }
   }

   bool Bounds3::intersect(
      const Ray& ray, 
      float &hit1, 
      float &hit2
   ) const {
      float tMin = -std::numeric_limits<float>::infinity();
      float tMax = std::numeric_limits<float>::infinity();

      if (_isInfinity) {
         hit1 = tMin;
         hit2 = tMax;
         return true;
      }

      for (auto axis : {Axis::X, Axis::Y, Axis::Z}) {
         double originCoord = ray.origin[axis];
         double dirCoord = ray.direction[axis];
         double minCoord = _min[axis];
         double maxCoord = _max[axis];

         if (std::abs(dirCoord) < 1e-6) {
            if (originCoord < minCoord || originCoord > maxCoord) {
               return false;
            }
         } else {
            double invDir = 1.0f / dirCoord;

            double t0 = (minCoord - originCoord) * invDir;
            double t1 = (maxCoord - originCoord) * invDir;

            if (invDir < 0.0f) 
               std::swap(t0, t1);

            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax < tMin ) return false;
         }
      }

      hit1 = tMin;
      hit2 = tMax;
      
      return true;
   }
}