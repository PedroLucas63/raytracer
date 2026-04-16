#ifndef GEOMETRIC_PRIMITIVE_HPP
#define GEOMETRIC_PRIMITIVE_HPP

#include "Objects/Primitive.hpp"
#include "Objects/Shapes/Shape.hpp"
#include "Objects/Materials/Material.hpp"
#include <memory>

namespace raytracer {
   class GeometricPrimitive : public Primitive {
      private:
         std::shared_ptr<Shape> _shape;
         std::shared_ptr<Material> _material;

      public:
         GeometricPrimitive(
            std::shared_ptr<Shape> shape, 
            std::shared_ptr<Material> material
         ): 
            _shape(shape), _material(material) {}
         ~GeometricPrimitive() = default;

         bool intersect(const Ray& ray) const override;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const override;

         const bool hasMaterial() const override;
         const std::shared_ptr<Material> getMaterial() const override;

         const Bounds3 getBounds() const override;
   };
}

#endif // !GEOMETRIC_PRIMITIVE_HPP