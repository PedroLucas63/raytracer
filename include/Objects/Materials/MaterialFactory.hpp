#ifndef OBJECT_MATERIAL_FACTORY_HPP
#define OBJECT_MATERIAL_FACTORY_HPP

#include "Objects/Materials/Material.hpp"
#include <memory>

namespace raytracer {
   class MaterialFactory {
      private:
         MaterialFactory() = default;

      public:
         static std::shared_ptr<Material> create(const ParamSet& params);
   };
}

#endif // !OBJECT_MATERIAL_FACTORY_HPP