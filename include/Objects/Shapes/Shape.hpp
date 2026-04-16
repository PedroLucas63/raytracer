#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "Math/Ray.hpp"
#include "Math/Bounds3.hpp"
#include "Objects/Surfel.hpp"
#include "Core/ParamSet.hpp"

namespace raytracer {
   class Shape {
      private:
         bool _flipNormals;
         
      public:
         Shape(bool flipNormals = false) : _flipNormals(flipNormals) {}
         virtual ~Shape() = default;

         bool flipNormals() const { return _flipNormals; }
         virtual bool intersect(const Ray& ray) const = 0;
         virtual bool intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const = 0;
         virtual Bounds3 getBounds() const = 0;
   };
}

#endif // !SHAPE_HPP