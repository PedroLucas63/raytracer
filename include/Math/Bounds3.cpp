#include "Bounds3.hpp"

namespace raytracer {
   const Point3& Bounds3::min() const {
      return _min;
   }

   const Point3& Bounds3::max() const {
      return _max;
   }

   Bounds3 Bounds3::merge(const Bounds3& point) const {
      Point3 newMin(std::min(_min.getX(), point._min.getX()),
                     std::min(_min.getY(), point._min.getY()),
                     std::min(_min.getZ(), point._min.getZ()));

      Point3 newMax(std::max(_max.getX(), point._max.getX()),
                     std::max(_max.getY(), point._max.getY()),
                     std::max(_max.getZ(), point._max.getZ()));

      return Bounds3(newMin, newMax);
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

   bool Bounds3::intersect(const Ray& ray, float &hit1, float &hit2) const {
      float tMin = -std::numeric_limits<float>::infinity();
      float tMax = std::numeric_limits<float>::infinity();

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
            if (tMax <= tMin ) return false;
         }
      }

      hit1 = tMin;
      hit2 = tMax;
      
      return true;
   }
}