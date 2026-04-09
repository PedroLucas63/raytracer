#include "ObjectFactory.hpp"
#include "Shapes/Sphere.hpp"

namespace raytracer {
   std::shared_ptr<Primitive> ObjectFactory::createPrimitive(
      const ParamSet& params, const Scene& scene
   ) {
      if (!params.has("type")) {
         throw std::invalid_argument("ObjectFactory requires a 'type' parameter");
      }

      if (!params.has("material")) {
         throw std::invalid_argument("ObjectFactory requires a 'material' parameter");
      }

      std::string material_name = params.retrieve<std::string>("material");
      std::shared_ptr<Material> material = scene.getMaterialAt(material_name);

      std::string type = params.retrieve<std::string>("type");
      if (type == "sphere") {
         return std::make_shared<Sphere>(params, material);
      } else {
         throw std::invalid_argument("Unknown primitive type: " + type);
      }
   }
}