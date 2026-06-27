#ifndef AGGREGATE_PRIMITIVE_HPP
#define AGGREGATE_PRIMITIVE_HPP

#include "Objects/Primitive.hpp"
#include "Math/Transform.hpp"
#include <vector>

namespace raytracer {
   class AggregatePrimitive : public Primitive {
      protected:
         using instance = std::pair<std::shared_ptr<Primitive>, std::shared_ptr<Transform>>; 
         using iterator = std::vector<instance>::iterator;

      public:
         AggregatePrimitive() : Primitive() {}
         virtual ~AggregatePrimitive() = default;

         const std::shared_ptr<Material> getMaterial() const override;
         const bool hasMaterial() const override;

         virtual void add(const instance& instance) = 0;
         virtual void merge(
            const std::shared_ptr<AggregatePrimitive>& other,
            const Transform& transform
         ) = 0;
         virtual void insert(iterator const& begin, iterator const& end) = 0;
   };
}

#endif // !AGGREGATE_PRIMITIVE_HPP