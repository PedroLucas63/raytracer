#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"

namespace raytracer {
   class Cylinder : public Shape {
      private:
         Point3 _base, _top;
         float _radius;

         bool intersect(const Ray& ray, float* tMin, float* tMax) const;
         bool intersectCaps(const Ray& ray, float* t, bool* hitBottom) const;
      
      public:
         Cylinder();
         Cylinder(const Point3& base, const Point3& top, float radius);
         Cylinder(const ParamSet& params);
         ~Cylinder() = default;

         bool intersect(const Ray& ray, const Transform& objToWorld) const override;
         bool intersectWithSurfel(const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf) const override;

         Bounds3 getBounds(const Transform& objToWorld) const override;
   };
}

#endif // CYLINDER_HPP