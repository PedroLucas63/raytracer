#include "Objects/Aggregate/AggregateFactory.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include "Objects/Aggregate/BVHAccel.hpp"
#include "Objects/Aggregate/LinearBVHAccel.hpp"


namespace raytracer {
   std::shared_ptr<AggregatePrimitive> AggregateFactory::create(
      const ParamSet& params
   ) {
      
      const std::string type = params.retrieveOrDefault<std::string>("type", "list");
      
      if (type == "list") {
         return std::make_shared<PrimitiveList>();
      } else if (type == "bvh") {
         return std::make_shared<BVHAccel>(params);
      } else if (type == "lbvh") {
         return std::make_shared<LinearBVHAccel>(params);
      }

      throw std::runtime_error("Unknown aggregate primitive type: " + type);
   }
}