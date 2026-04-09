#ifndef OBJECT_FACTORY_HPP

#include "Primitive.hpp"
#include "Scene/Scene.hpp"
#include <memory>

namespace raytracer {
   class ObjectFactory {
      private:
         ObjectFactory() = default;

      public:
         static std::shared_ptr<Primitive> createPrimitive(
            const ParamSet& params, const Scene& scene
         );
   };
}

#endif // !OBJECT_FACTORY_HPP