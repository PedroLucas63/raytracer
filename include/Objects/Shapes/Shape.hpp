#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "Math/Ray.hpp"
#include "Math/Bounds3.hpp"
#include "Objects/Surfel.hpp"
#include "Core/ParamSet.hpp"
#include "Math/Transform.hpp"

namespace raytracer {
   class Shape {
      public:
         const bool flipNormals = false;
          
         Shape(bool flipNormals = false) : flipNormals(flipNormals) {}
         virtual ~Shape() = default;

         virtual bool intersect(const Ray& ray, const Transform& objToWorld) const = 0;
         virtual bool intersectWithSurfel(const Ray& ray, const Transform& objToWorld, float *tHit, Surfel* surfel) const = 0;
         virtual Bounds3 getBounds(const Transform& objToWorld) const = 0;
   };
}

#endif // !SHAPE_HPP