#include "Objects/Shapes/Sphere.hpp"
#include <cmath>

namespace raytracer {
   std::pair<float, float> Sphere::calculateIntersectPoints(
      const Ray& ray
   ) const {
      auto oc = ray.origin.toVector();

      auto A = ray.direction.dot(ray.direction);
      auto B = 2.0f * oc.dot(ray.direction);
      auto C = oc.dot(oc) - 1.0f;

      auto delta = B * B - 4 * A * C;

      if (delta < 0) return {-1, -1};
      
      auto square_delta = std::sqrt(delta);
      auto t1 = (-B - square_delta) / (2.0f * A);
      auto t2 = (-B + square_delta) / (2.0f * A);

      return {t1, t2};
   }

   bool Sphere::intersect(const Ray& ray, const Transform& objToWorld) const {
      auto localRay = objToWorld(ray, true);
      auto [t1, t2] = calculateIntersectPoints(localRay);
      return (t1 >= localRay.t_min && t1 <= localRay.t_max) || 
         (t2 >= localRay.t_min && t2 <= localRay.t_max);
   }

   bool Sphere::intersectWithSurfel(
      const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf
   ) const {
      auto localRay = objToWorld(ray, true);
      auto [t1, t2] = calculateIntersectPoints(localRay);

      float t = std::numeric_limits<float>::infinity();

      if (t1 >= localRay.t_min && t1 <= localRay.t_max)
         t = t1;

      if (t2 >= localRay.t_min &&
         t2 <= localRay.t_max &&
         t2 < t)
      {
         t = t2;
      }

      if (!std::isfinite(t))
         return false;

      auto pLocal = localRay.origin + localRay.direction * t;
      auto nLocal = pLocal.toVector().normalize();
      auto pWorld = objToWorld(pLocal);
      auto nWorld = objToWorld.applyNormal(nLocal).normalize();

      if (sf) {
         *sf = Surfel(t, pWorld, nullptr, nWorld, -ray.direction.normalize());
      }

      if (tHit) {
         *tHit = t;
      }

      return true;
   }

   Bounds3 Sphere::getBounds(const Transform& objToWorld) const {
      auto center = POINT3_ZERO;
      auto p1 = center - VECTOR3_ONE;
      auto p2 = center + VECTOR3_ONE;

      auto b = Bounds3(p1, p2);
      return objToWorld(b);
   }
}