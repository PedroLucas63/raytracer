#ifndef OBJECT_MATERIAL_FACTORY_HPP
#define OBJECT_MATERIAL_FACTORY_HPP

#include "Material.hpp"

namespace raytracer {
   class MaterialFactory {
      private:
         MaterialFactory() = default;

      public:
         static std::shared_ptr<Material> build(const ParamSet& params);
   };
}

#endif // !OBJECT_MATERIAL_FACTORY_HPP