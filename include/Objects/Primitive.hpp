#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "Math/Ray.hpp"
#include "Material.hpp"
#include "Surfel.hpp"
#include <memory>

namespace raytracer {
   class Primitive {
      protected:
         std::shared_ptr<Material> _material;

      public:
         Primitive(std::shared_ptr<Material> material) : _material(material) {}
         virtual ~Primitive() = default;
         virtual bool intersect(const Ray& ray) const = 0;
         virtual bool intersectWithSurfel(const Ray& ray, Surfel* sf) const = 0;
         virtual const bool hasMaterial() const { return _material != nullptr; }
         virtual const std::shared_ptr<Material> getMaterial() const {
            if (!hasMaterial()) {
               throw std::runtime_error("Primitive has no material assigned");
            }
            return _material;
         };
   };
}

#endif // !PRIMITIVE_HPP