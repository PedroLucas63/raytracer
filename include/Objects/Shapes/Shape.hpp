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
         const Transform* objToWorld{ nullptr };
         const Transform* worldToObj{ nullptr };
         const bool flipNormals{ false };
          
         Shape(const Transform* objToWorld = nullptr, const Transform* worldToObj = nullptr, bool flipNormals = false) 
            : objToWorld(objToWorld), worldToObj(worldToObj), flipNormals(flipNormals) {}

         Shape(bool flipNormals) : flipNormals(flipNormals) {}

         virtual ~Shape() = default;

         virtual bool intersect(const Ray& ray) const = 0;
         virtual bool intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const = 0;
         virtual Bounds3 getBounds() const = 0;
   };
}

#endif // !SHAPE_HPP