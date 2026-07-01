#include "Objects/Aggregate/PrimitiveList.hpp"
#include <cmath>

namespace raytracer {
   bool PrimitiveList::intersect(const Ray& ray, const Transform& transform) const {
      for (auto instance : _instances) {
         auto [primitive, instanceTransform] = instance;
         Transform composed = transform * (*instanceTransform);
         if (primitive->intersect(ray, composed)) 
            return true;
      }

      return false;
   }

   bool PrimitiveList::intersectWithSurfel(
      const Ray& ray, const Transform& transform, Surfel* sf
   ) const {
      bool hit = false;

      Surfel minimumSf = Surfel();
      minimumSf.t = INFINITY;

      for (auto& instance : _instances) {
         auto [primitive, instanceTransform] = instance;
         Surfel actualSf = Surfel();
         Transform composed = transform * (*instanceTransform);
         if (primitive->intersectWithSurfel(ray, composed, &actualSf)) {
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

   const Bounds3 PrimitiveList::getBounds(const Transform& transform) const {
      if (_instances.empty()) {
         return Bounds3(Point3(0, 0, 0), Point3(0, 0, 0));
      }

      Bounds3 totalBounds = _instances[0].first->getBounds(transform * (*_instances[0].second));
      for (size_t i = 1; i < _instances.size(); ++i) {
         auto [primitive, instanceTransform] = _instances[i];
         auto composedTransform = transform * (*instanceTransform);
         auto instanceBounds = primitive->getBounds(composedTransform);
         totalBounds = totalBounds.merge(instanceBounds);
      }
      return totalBounds;
   } 

   void PrimitiveList::add(const instance& instance) {
      _instances.push_back(instance);
   }

   void PrimitiveList::merge(
      const std::shared_ptr<AggregatePrimitive>& other,
      const Transform& transform
   ) {
      auto otherList = std::dynamic_pointer_cast<PrimitiveList>(other);
      if (!otherList)
         throw std::invalid_argument("The other aggregate primitive must be a PrimitiveList");
      
      for (const auto& instance : otherList->_instances) {
         auto [primitive, instanceTransform] = instance;
         auto combinedTransform = std::make_shared<Transform>(transform * (*instanceTransform));
         _instances.push_back({primitive, combinedTransform});
      }
   }

   void PrimitiveList::insert(
      iterator const& begin, 
      iterator const& end
   ) {
      _instances.insert(_instances.end(), begin, end);
   }
}