#ifndef AGGREGATE_FACTORY_HPP
#define AGGREGATE_FACTORY_HPP

#include "Objects/Aggregate/AggregatePrimitive.hpp"

namespace raytracer {
   class AggregateFactory {
      private:
         AggregateFactory() = default;

      public:
         static std::shared_ptr<AggregatePrimitive> create(
            const ParamSet& params
         );
   };
}

#endif // !AGGREGATE_FACTORY_HPP