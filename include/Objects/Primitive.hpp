#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "Math/Ray.hpp"
#include "Math/Bounds3.hpp"
#include "Objects/Materials/Material.hpp"
#include "Objects/Surfel.hpp"
#include <memory>

namespace raytracer {
   class Primitive {
      public:
         Primitive() {}
         virtual ~Primitive() = default;

         virtual bool intersect(const Ray& ray) const = 0;
         virtual bool intersectWithSurfel(const Ray& ray, Surfel* sf) const = 0;

         virtual const bool hasMaterial() const = 0;
         virtual const std::shared_ptr<Material> getMaterial() const = 0;

         virtual const Bounds3 getBounds() const = 0;
   };
}

#endif // !PRIMITIVE_HPP