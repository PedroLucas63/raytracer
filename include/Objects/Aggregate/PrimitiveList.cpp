#include "Objects/Aggregate/PrimitiveList.hpp"
#include <cmath>

namespace raytracer {
   bool PrimitiveList::intersect(const Ray& ray) const {
      for (auto primitive : _primitives) {
         if (primitive->intersect(ray)) 
            return true;
      }

      return false;
   }

   bool PrimitiveList::intersectWithSurfel(const Ray& ray, Surfel* sf) const {
      bool hit = false;

      Surfel minimumSf = Surfel();
      minimumSf.t = INFINITY;

      for (auto& primitive : _primitives) {
         Surfel actualSf = Surfel();
         if (primitive->intersectWithSurfel(ray, &actualSf)) {
            hit = true;
            if (actualSf.t < minimumSf.t) {
               minimumSf = actualSf;
            }
         }
      }

      if (sf && hit) {
         *sf = minimumSf;
      }

      return hit;
   }

   const Bounds3 PrimitiveList::getBounds() const {
      if (_primitives.empty()) {
         return Bounds3(Point3(0, 0, 0), Point3(0, 0, 0));
      }

      double minX = INFINITY, minY = INFINITY, minZ = INFINITY;
      double maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

      for (auto& primitive : _primitives) {
         auto bound = primitive->getBounds();

         minX = std::min(minX, bound.min().getX());
         minY = std::min(minY, bound.min().getY());
         minZ = std::min(minZ, bound.min().getZ());

         maxX = std::max(maxX, bound.max().getX());
         maxY = std::max(maxY, bound.max().getY());
         maxZ = std::max(maxZ, bound.max().getZ());
      }

      auto bound = Bounds3(
         Point3(minX, minY, minZ),
         Point3(maxX, maxY, maxZ)
      );

      return bound;
   } 

      void PrimitiveList::add(const std::shared_ptr<Primitive>& primitive) {
            _primitives.push_back(primitive);
         }


      void PrimitiveList::merge(const std::shared_ptr<PrimitiveList>& other) {
         _primitives.insert(
            _primitives.end(),
            other->_primitives.begin(),
            other->_primitives.end()
         );
      }
}