#include "BackgroundFactory.hpp"
#include "BackgroundColor.hpp"
#include "BackgroundImage.hpp"

namespace raytracer {
   std::string BackgroundFactory::getAndValidateBackgroundType(
      const ParamSets& params
   ) {
      auto it = params.find("background");
      if (it == params.end())
         throw std::runtime_error("Background parameters not found");
      
      auto paramSet = it->second;
      if (paramSet.has("type"))
         return paramSet.retrieve<std::string>("type");

      throw std::runtime_error("Background type not specified");
   }

   std::shared_ptr<Background> BackgroundFactory::create(const ParamSets& params) {
      std::string type = getAndValidateBackgroundType(params);
      if (type == "image")
         return std::make_shared<BackgroundImage>(params);
      else
         return std::make_shared<BackgroundColor>(params);
   }
}