#ifndef PLAN_HPP
#define PLAN_HPP

#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include "Objects/Shapes/Shape.hpp"

namespace raytracer {
   class Plane : public Shape {
      private:
         float getIntersection(const Ray& ray) const;
         const static Vector3 _norm;
         
      public:
         Plane() = default;
         ~Plane() = default;

         bool intersect(const Ray& ray, const Transform& objToWorld) const;
         bool intersectWithSurfel(const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf) const;

         Bounds3 getBounds(const Transform& objToWorld) const override;
   };
}

#endif // !PLAN_HPP