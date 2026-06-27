#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"
#include <tuple>

namespace raytracer {
   class Sphere : public Shape {
      private:
         std::pair<float, float> calculateIntersectPoints(const Ray& ray) const;

      public:
         Sphere() = default;
         ~Sphere() = default;

         bool intersect(const Ray& ray, const Transform& objToWorld) const override;
         bool intersectWithSurfel(const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf) const override;

         Bounds3 getBounds(const Transform& objToWorld) const override;
   };
}

#endif // !SPHERE_HPP