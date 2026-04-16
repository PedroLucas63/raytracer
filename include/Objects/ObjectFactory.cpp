#include "ObjectFactory.hpp"
#include "Shapes/Sphere.hpp"
#include "Shapes/Plan.hpp"
#include <iostream>

namespace raytracer {
   std::shared_ptr<Primitive> ObjectFactory::createPrimitive(
      const ParamSet& params, const Scene& scene
   ) {
      if (!params.has("type")) {
         throw std::invalid_argument("ObjectFactory requires a 'type' parameter");
      }

      std::shared_ptr<Material> material;
      if (!params.has("material")) {
         material = scene.getLastMaterial();
      } else {
         std::string material_name = params.retrieve<std::string>("material");
         if (material_name.starts_with("__")) {
            throw std::invalid_argument("Cannot reference anonymous material: " + material_name);
         }
         material = scene.getMaterialAt(material_name);
      }

      if (material == nullptr)
         std::cerr << "[WARN]: Primitive created without material" << std::endl;

      std::string type = params.retrieve<std::string>("type");
      if (type == "sphere") {
         return std::make_shared<Sphere>(params, material);
      } else if (type == "plan") {
         return std::make_shared<Plan>(params, material);
      } else {
         throw std::invalid_argument("Unknown primitive type: " + type);
      }
   }
}