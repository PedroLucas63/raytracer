#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Objects/Primitive.hpp"
#include "Math/Point3.hpp"
#include <tuple>

namespace raytracer {
   class Sphere : public Primitive {
      private:
         Point3 _center;
         float _radius;

         std::pair<float, float> calculateIntersectPoints(const Ray& ray) const;

      public:
         Sphere(const Point3& center, float radius): 
            _center(center), _radius(radius) {}
         ~Sphere() = default;

         bool intersect(const Ray& ray) const override;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const override;
   };
}

#endif // !SPHERE_HPP