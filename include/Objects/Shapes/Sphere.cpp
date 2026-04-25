#include "Objects/Shapes/Sphere.hpp"
#include <cmath>

namespace raytracer {

   Sphere::Sphere(const ParamSet& params) : Shape() {
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
      auto t2 = (-B + square_delta) / (2 * A);

      return {t1, t2};
   }

   bool Sphere::intersect(const Ray& ray) const {
      auto [t1, t2] = calculateIntersectPoints(ray);
      return (t1 >= ray.t_min && t1 <= ray.t_max) || 
         (t2 >= ray.t_min && t2 <= ray.t_max);
   }

   bool Sphere::intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const {
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
         if (!setT) {
            t = t2;
            setT = true;
         } else if (t2 < t) {
            t = t2;
         }
      }

      if (sf) {
         auto intersectPoint = ray.origin + (ray.direction * t);
         auto normal = (intersectPoint - _center).normalize();
         *sf = Surfel(t, intersectPoint, nullptr, normal);
      }

      if (tHit) {
         *tHit = t;
      }

      return true;
   }

   Bounds3 Sphere::getBounds() const {
      auto vec = Vector3(_radius, _radius, _radius);
      auto point1 = _center - vec;
      auto point2 = _center + vec;
      return Bounds3(point1, point2);
   }
}