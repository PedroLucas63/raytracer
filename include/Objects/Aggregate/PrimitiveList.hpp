#ifndef PRIMITIVE_LIST_HPP
#define PRIMITIVE_LIST_HPP

#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Primitive.hpp"
#include "Math/Transform.hpp"
#include <vector>
#include <memory>
#include <tuple>

namespace raytracer {
   class PrimitiveList : public AggregatePrimitive {
      private:
         std::vector<instance> _instances;

      public:
         PrimitiveList() : AggregatePrimitive() {}
         ~PrimitiveList() = default;

         bool intersect(const Ray& ray, const Transform& transform) const override;
         bool intersectWithSurfel(const Ray& ray, const Transform& transform, Surfel* sf) const override;

         const Bounds3 getBounds(const Transform& transform) const override; 

         void add(const instance& instance) override;
         void merge(
            const std::shared_ptr<AggregatePrimitive>& other,
            const Transform& transform
         ) override;
         void insert(iterator const& begin, iterator const& end) override;

         auto size() { return _instances.size(); }
         auto begin() { return _instances.begin(); }
         auto end() { return _instances.end(); }
         void clear() { _instances.clear(); }
   };
}

#endif // !PRIMITIVE_LIST_HPP