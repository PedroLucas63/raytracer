#ifndef OBJECT_FACTORY_HPP

#include "Objects/Primitive.hpp"
#include "Scene/Scene.hpp"
#include <memory>
#include <string>

namespace raytracer {
   class PrimitiveFactory {
      private:
         PrimitiveFactory() = default;

         static bool isGeometricPrimitive(const std::string& type);
         static std::shared_ptr<Primitive> createGeometricPrimitive(
            const ParamSet& params, const Scene& scene
         );

      public:
         static std::shared_ptr<Primitive> create(
            const ParamSet& params, const Scene& scene
         );
   };
}

#endif // !OBJECT_FACTORY_HPP