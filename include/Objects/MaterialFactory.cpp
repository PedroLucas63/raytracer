#include <memory>
#include "MaterialFactory.hpp"

namespace raytracer {
   std::shared_ptr<Material> MaterialFactory::create(const ParamSet& params) {
      if (!params.has("type")) {
         throw std::invalid_argument("MaterialFactory requires a 'type' parameter");
      }

      if (!params.has("name")) {
         throw std::invalid_argument("MaterialFactory requires a 'name' parameter");
      }

      std::string type = params.retrieve<std::string>("type");      
      if (type == "color") {
         return std::make_shared<ColorMaterial>(params);
      } else if (type == "grid") {
         return std::make_shared<GridMaterial>(params);
      } else {
         throw std::invalid_argument("Unknown material type: " + type);
      }
   }
}