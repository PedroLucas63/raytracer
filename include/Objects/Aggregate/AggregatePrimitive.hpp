#ifndef AGGREGATE_PRIMITIVE_HPP
#define AGGREGATE_PRIMITIVE_HPP

#include "Objects/Primitive.hpp"

namespace raytracer {
   class AggregatePrimitive : public Primitive {
      public:
         AggregatePrimitive() : Primitive() {}
         virtual ~AggregatePrimitive() = default;

         const std::shared_ptr<Material> getMaterial() const override;
         const bool hasMaterial() const override;
   };
}

#endif // !AGGREGATE_PRIMITIVE_HPP