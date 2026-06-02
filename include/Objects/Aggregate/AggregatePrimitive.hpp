#ifndef AGGREGATE_PRIMITIVE_HPP
#define AGGREGATE_PRIMITIVE_HPP

#include "Objects/Primitive.hpp"
#include <vector>

namespace raytracer {
   class AggregatePrimitive : public Primitive {
      protected:
         using iterator = std::vector<std::shared_ptr<Primitive>>::iterator;

      public:
         AggregatePrimitive() : Primitive() {}
         virtual ~AggregatePrimitive() = default;

         const std::shared_ptr<Material> getMaterial() const override;
         const bool hasMaterial() const override;

         virtual void add(const std::shared_ptr<Primitive>& primitive) = 0;
         virtual void merge(const std::shared_ptr<AggregatePrimitive>& other) = 0;
         virtual void insert(iterator const& begin, iterator const& end) = 0;
   };
}

#endif // !AGGREGATE_PRIMITIVE_HPP