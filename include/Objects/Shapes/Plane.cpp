#include "Objects/Shapes/Plane.hpp"
#include <cmath>

namespace raytracer {
   const Vector3 Plane::_norm = VECTOR3_UNIT_Y;

   float Plane::getIntersection(const Ray& ray) const {
      const float EPS = 1e-6;
      float denom = _norm.dot(ray.direction);

      if (fabs(denom) < EPS) {
         return -1;
      }
      
      return _norm.dot(-ray.origin.toVector()) / denom;
   }

   bool Plane::intersect(const Ray& ray, const Transform& objToWorld) const {
      auto localRay = objToWorld(ray, true);
      float t = getIntersection(localRay);
      return t >= localRay.t_min && t <= localRay.t_max;
   }

   bool Plane::intersectWithSurfel(
      const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf
   ) const {
      auto localRay = objToWorld(ray, true);
      auto t = getIntersection(localRay);

      if (t < localRay.t_min || t > localRay.t_max) return false;

      if (tHit) {
         *tHit = t;
      }

      if (sf) {
         auto pLocal = localRay.origin + (localRay.direction * t);
         Vector3 nLocal = (_norm.dot(localRay.direction) < 0)
            ? _norm
            : -_norm;
         auto pWorld = objToWorld(pLocal);
         auto nWorld = objToWorld.applyNormal(nLocal).normalize();
         *sf = Surfel(t, pWorld, nullptr, nWorld, -ray.direction.normalize());
      }

      return true;
   }

   Bounds3 Plane::getBounds(const Transform& objToWorld) const {
      return Bounds3::infinite();
   }
}