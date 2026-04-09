#ifndef OBJECT_FACTORY_HPP

#include "Primitive.hpp"
#include <memory>

namespace raytracer {
   class ObjectFactory {
      private:
         ObjectFactory() = default;

      public:
         static std::shared_ptr<Primitive> buildPrimitive(const ParamSet& params);
   };
}

#endif // !OBJECT_FACTORY_HPP