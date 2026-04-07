#include "Sphere.hpp"
#include <cmath>

namespace raytracer {

   std::pair<float, float> Sphere::calculateIntersectPoints(const Ray& ray) const {
      auto oc = ray.origin - _center;

      auto A = ray.direction.dot(ray.direction);
      auto B = 2 * oc.dot(ray.direction);
      auto C = oc.dot(oc) - _radius * _radius;

      auto delta = B * B - 4 * A * C;

      if (delta < 0) return {-1, -1};
      
      auto square_delta = std::sqrt(delta);
      auto t1 = (-B - square_delta) / (2 * A);
      auto t2 = -1;

      if (delta != 0)
         t2 = (-B + square_delta) / (2 * A);

      return {t1, t2};
   }

   bool Sphere::intersect(const Ray& ray) const {
      auto [t1, t2] = calculateIntersectPoints(ray);
      return (t1 >= ray.t_min && t1 <= ray.t_max) || 
         (t2 >= ray.t_min && t2 <= ray.t_max);
   }

   bool Sphere::intersectWithSurfel(const Ray& ray, Surfel* sf) const {
      // TODO: Update surfel
      return intersect(ray);
   }
}