#include "Objects/Shapes/Plan.hpp"
#include <cmath>

namespace raytracer {
   Plan::Plan(const ParamSet& params) {
      if (!params.has("origin")) {
         throw std::invalid_argument("Plan requires a 'origin' parameter");
      }
      if (!params.has("norm")) {
         throw std::invalid_argument("Plan requires a 'norm' parameter");
      }

      _origin = params.retrieve<Point3>("origin");
      _norm = params.retrieve<Vector3>("norm").normalize();
   }

   float Plan::getIntersection(const Ray& ray) const {
      const float EPS = 1e-6;
      float denom = _norm.dot(ray.direction);

      if (fabs(denom) < EPS) {
         float dist = _norm.dot(ray.origin - _origin);
         return fabs(dist) < EPS ? 0 : -1;
      }
      
      return _norm.dot(_origin - ray.origin) / denom;
   }

   bool Plan::intersect(const Ray& ray) const {
      float t = getIntersection(ray);
      return t >= ray.t_min && t <= ray.t_max;
   }

   bool Plan::intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const {
      auto t = getIntersection(ray);

      if (t < ray.t_min || t > ray.t_max) return false;

      if (tHit) {
         *tHit = t;
      }

      if (sf) {
         auto intersectPoint = ray.origin + (ray.direction * t);
         *sf = Surfel(t, intersectPoint);
      }

      return true;
   }

   Bounds3 Plan::getBounds() const {
      auto point1 = Point3(-INFINITY, -INFINITY, -INFINITY);
      auto point2 = Point3(INFINITY, INFINITY, INFINITY);
      return Bounds3(point1, point2);
   }
}