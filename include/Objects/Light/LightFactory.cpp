#include "Objects/Light/LightFactory.hpp"
#include "Objects/Light/PointLight.hpp"
#include "Objects/Light/DirectionalLight.hpp"
#include "Objects/Light/AmbientLight.hpp"

namespace raytracer {
   std::string LightFactory::getType(const ParamSet& params) {
      if (!params.has("type")) {
         throw std::invalid_argument("LightFactory requires a 'type' parameter");
      }
      return params.retrieve<std::string>("type");
   }

   bool LightFactory::validateType(const std::string& type) {
      return type == "point" || type == "directional" || type == "ambient";
   }
   
   std::shared_ptr<Light> LightFactory::create(const ParamSet& params) {
      std::string type = getType(params);
      if (!validateType(type)) {
         throw std::invalid_argument("Unknown light type: " + type);
      }

      if (type == "point") {
         return std::make_shared<PointLight>(params);
      } else if (type == "directional") {
         return std::make_shared<DirectionalLight>(params);
      } else if (type == "ambient") {
         return std::make_shared<AmbientLight>(params);
      } else {
         throw std::invalid_argument("Unknown light type: " + type);
      }
   }
}