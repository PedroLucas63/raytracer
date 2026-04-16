#ifndef PLAN_HPP
#define PLAN_HPP

#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include "Objects/Shapes/Shape.hpp"

namespace raytracer {
   class Plan : public Shape {
      private:
         Point3 _origin;
         Vector3 _norm;

         float getIntersection(const Ray& ray) const;
         
      public:
         Plan(const Point3& origin, const Vector3& vec)
            : _origin(origin), _norm(vec.normalize()) {}
         Plan(const ParamSet& params);

         ~Plan() = default;

         bool intersect(const Ray& ray) const;
         bool intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const;

         Bounds3 getBounds() const override;
   };
}

#endif // !PLAN_HPP