#include <memory>
#include "MaterialFactory.hpp"

namespace raytracer {
   std::shared_ptr<Material> MaterialFactory::create(const ParamSet& params) {
      if (!params.has("type")) {
         throw std::invalid_argument("MaterialFactory requires a 'type' parameter");
      }

      std::string type = params.retrieve<std::string>("type");      
      if (type == "flat") {
         return std::make_shared<FlatMaterial>(params);
      } else if (type == "grid") {
         return std::make_shared<GridMaterial>(params);
      } else {
         throw std::invalid_argument("Unknown material type: " + type);
      }
   }
}