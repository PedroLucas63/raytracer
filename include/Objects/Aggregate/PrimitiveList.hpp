#ifndef PRIMITIVE_LIST_HPP
#define PRIMITIVE_LIST_HPP

#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Primitive.hpp"
#include <vector>
#include <memory>

namespace raytracer {
   class PrimitiveList : public AggregatePrimitive {
      private:
         std::vector<std::shared_ptr<Primitive>> _primitives;

      public:
         PrimitiveList() : AggregatePrimitive() {}
         ~PrimitiveList() = default;

         bool intersect(const Ray& ray) const override;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const override;

         const Bounds3 getBounds() const override; 

         void add(const std::shared_ptr<Primitive>& primitive);

         void merge(const std::shared_ptr<PrimitiveList>& other);
   };
}

#endif // !PRIMITIVE_LIST_HPP