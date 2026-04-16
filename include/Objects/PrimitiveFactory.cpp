#include "Objects/PrimitiveFactory.hpp"
#include "Objects/GeometricPrimitive.hpp"
#include "Objects/Shapes/Sphere.hpp"
#include "Objects/Shapes/Plan.hpp"
#include "Objects/Shapes/Shape.hpp"
#include <iostream>

namespace raytracer {
   bool PrimitiveFactory::isGeometricPrimitive(const std::string& type) {
      return type == "sphere" || type == "plan";
   }

   std::shared_ptr<Primitive> PrimitiveFactory::createGeometricPrimitive(
      const ParamSet& params, const Scene& scene
   ) {
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
      std::shared_ptr<Shape> shape;
      if (type == "sphere") {
         shape = std::make_shared<Sphere>(params);
      } else if (type == "plan") {
         shape = std::make_shared<Plan>(params);
      } else {
         throw std::invalid_argument("Unknown primitive type: " + type);
      }

      return std::make_shared<GeometricPrimitive>(shape, material);
   }

   std::shared_ptr<Primitive> PrimitiveFactory::create(
      const ParamSet& params, const Scene& scene
   ) {
      if (!params.has("type")) {
         throw std::invalid_argument("ObjectFactory requires a 'type' parameter");
      }

      if (isGeometricPrimitive(params.retrieve<std::string>("type"))) {
         return createGeometricPrimitive(params, scene);
      } else {
         throw std::invalid_argument("Invalid primitive 'type'");
      }
   }
}