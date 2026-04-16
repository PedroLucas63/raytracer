#include "Objects/GeometricPrimitive.hpp"

namespace raytracer {
   bool GeometricPrimitive::intersect(const Ray& ray) const {
      return _shape->intersect(ray);
   }

   bool GeometricPrimitive::intersectWithSurfel(const Ray& ray, Surfel* sf) const {
      auto result = _shape->intersectWithSurfel(ray, nullptr, sf);
      if (result && sf) {
         sf->material = _material;
      }
      return result;
   }

   const bool GeometricPrimitive::hasMaterial() const {
      return _material != nullptr;
   }

   const std::shared_ptr<Material> GeometricPrimitive::getMaterial() const {
      if (!hasMaterial()) {
         throw std::runtime_error("GeometricPrimitive has no material assigned");
      }
      return _material;
   }

   const Bounds3 GeometricPrimitive::getBounds() const {
      return _shape->getBounds();
   }
}