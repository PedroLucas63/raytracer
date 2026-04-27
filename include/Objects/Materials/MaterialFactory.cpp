#include <memory>
#include "Objects/Materials/MaterialFactory.hpp"
#include "Objects/Materials/FlatMaterial.hpp"
#include "Objects/Materials/GridMaterial.hpp"
#include "Objects/Materials/BlinnMaterial.hpp"

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
      } else if (type == "blinn") {
         return std::make_shared<BlinnMaterial>(params);
      } else {
         throw std::invalid_argument("Unknown material type: " + type);
      }
   }
}