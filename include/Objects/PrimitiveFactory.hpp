#ifndef OBJECT_FACTORY_HPP

#include "Objects/Primitive.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include "Core/GraphicsState.hpp"
#include <memory>
#include <string>

namespace raytracer {
   class PrimitiveFactory {
      private:
         PrimitiveFactory() = default;

         static bool isGeometricPrimitive(const std::string& type);
         static std::shared_ptr<PrimitiveList> createGeometricPrimitive(
            const ParamSet& params, const GraphicsState& graphicsState
         );

      public:
         static std::shared_ptr<PrimitiveList> create(
            const ParamSet& params, const GraphicsState& graphicsState
         );
   };
}

#endif // !OBJECT_FACTORY_HPP