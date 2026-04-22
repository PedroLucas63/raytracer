#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include <stdexcept>

namespace raytracer {
   const std::shared_ptr<Material> AggregatePrimitive::getMaterial() const {
      throw std::logic_error("AggregatePrimitive does not have a material");
   }

   const bool AggregatePrimitive::hasMaterial() const {
      return false;
   }
}