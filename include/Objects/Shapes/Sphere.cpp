#include "Sphere.hpp"
#include <cmath>

namespace raytracer {

   Sphere::Sphere(const ParamSet& params, std::shared_ptr<Material> material) : Primitive(material) {
      if (!params.has("origin")) {
         throw std::invalid_argument("Sphere requires a 'origin' parameter");
      }
      if (!params.has("radius")) {
         throw std::invalid_argument("Sphere requires a 'radius' parameter");
      }

      _origin = params.retrieve<Point3>("origin");
      _radius = params.retrieve<float>("radius");
   }

   std::pair<float, float> Sphere::calculateIntersectPoints(const Ray& ray) const {
      auto oc = ray.origin - _origin;

      auto d = ray.direction.normalize();
      auto proj = oc.dot(d);
      auto oc_perp = oc - d * proj;
      auto delta_stable = _radius * _radius - oc_perp.lengthSquared();

      if (delta_stable < 0) return {-1, -1};

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
      auto [t1, t2] = calculateIntersectPoints(ray);

      auto result = (t1 >= ray.t_min && t1 <= ray.t_max) || 
         (t2 >= ray.t_min && t2 <= ray.t_max);
      
      if (!result) return false;

      auto t = -1.0f;
      bool setT = false;
      if (t1 >= ray.t_min && t1 <= ray.t_max) {
         t = t1;
         setT = true;
      }
      
      if (t2 >= ray.t_min && t2 <= ray.t_max) {
         t = setT && t2 < t ? t2 : t;
      }

      if (sf) {
         auto intersectPoint = ray.origin + (ray.direction * t);
         *sf = Surfel(t, intersectPoint, _material);
      }

      return true;
   }
}