#include "Sphere.hpp"
#include <cmath>

namespace raytracer {

   Sphere::Sphere(const ParamSet& params, std::shared_ptr<Material> material) : Primitive(material) {
      if (!params.has("center")) {
         throw std::invalid_argument("Sphere requires a 'center' parameter");
      }
      if (!params.has("radius")) {
         throw std::invalid_argument("Sphere requires a 'radius' parameter");
      }

      _center = params.retrieve<Point3>("center");
      _radius = params.retrieve<float>("radius");
   }

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