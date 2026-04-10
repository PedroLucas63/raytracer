#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Objects/Primitive.hpp"
#include "Math/Point3.hpp"
#include <tuple>

namespace raytracer {
   class Sphere : public Primitive {
      private:
         Point3 _origin;
         float _radius;

         std::pair<float, float> calculateIntersectPoints(const Ray& ray) const;

      public:
         Sphere(const Point3& origin, float radius, std::shared_ptr<Material> material): 
            Primitive(material), _origin(origin), _radius(radius) {}
         Sphere(const ParamSet& params, std::shared_ptr<Material> material);
         ~Sphere() = default;

         bool intersect(const Ray& ray) const override;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const override;
   };
}

#endif // !SPHERE_HPP