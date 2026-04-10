#ifndef PLAN_HPP
#define PLAN_HPP

#include "Objects/Primitive.hpp"
#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   class Plan : public Primitive {
      private:
         Point3 _origin;
         Vector3 _norm;

         float getIntersection(const Ray& ray) const;
         
      public:
         Plan(const ParamSet& params, std::shared_ptr<Material> material);
         Plan(const Point3& origin, const Vector3& vec, std::shared_ptr<Material> material)
            : Primitive(material), _origin(origin), _norm(vec.normalize()) {}
         ~Plan() = default;

         bool intersect(const Ray& ray) const;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const;
   };
}

#endif // !PLAN_HPP