#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "Math/Ray.hpp"
#include "Material.hpp"
#include "Surfel.hpp"
#include <memory>

namespace raytracer {
   class Primitive {
      private:
         std::shared_ptr<Material> _material;

      public:
         virtual ~Primitive() = default;
         virtual bool intersect(const Ray& ray) const = 0;
         virtual bool intersectWithSurfel(const Ray& ray, Surfel* sf) const = 0;
         virtual const Material* getMaterial() const {
            return _material.get();
         };
   };
}

#endif // !PRIMITIVE_HPP