#include "Objects/PrimitiveFactory.hpp"
#include "Objects/GeometricPrimitive.hpp"
#include "Objects/Shapes/Sphere.hpp"
#include "Objects/Shapes/Plane.hpp"
#include "Objects/Shapes/Box.hpp"
#include "Objects/Shapes/Shape.hpp"
#include "Objects/Shapes/Triangle.hpp"
#include "Objects/Shapes/Cylinder.hpp"
#include "Api/Api.hpp"
#include <iostream>

namespace raytracer {
   bool PrimitiveFactory::isGeometricPrimitive(const std::string& type) {
      return type == "sphere" || type == "plane" || type == "box" || type == "trianglemesh" || type == "cylinder";
   }

   std::shared_ptr<PrimitiveList> PrimitiveFactory::createGeometricPrimitive(
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

      const Transform* objToWorld = nullptr;
      const Transform* worldToObj = nullptr;
      bool flipNormals = false;
      Api::getCurrentTransform(&objToWorld, &worldToObj, &flipNormals);

      auto primitiveList = std::make_shared<PrimitiveList>();
      std::string type = params.retrieve<std::string>("type");
      std::shared_ptr<Shape> shape;
      if (type == "sphere") {
         shape = std::make_shared<Sphere>(params);
      } else if (type == "plane") {
         shape = std::make_shared<Plane>(params);
      } else if(type == "box"){
         shape = std::make_shared<Box>(params);
      } else if (type == "trianglemesh") {
         auto shapes = std::make_shared<TriangleMesh>(params);

         for (auto& shape : shapes->makeTriangules(objToWorld, worldToObj, flipNormals)) {
            primitiveList->add(
               std::make_shared<GeometricPrimitive>(shape, material)
            );
         }

         return primitiveList;
      } else if (type == "cylinder") {
         shape = std::make_shared<Cylinder>(params);
      } else {
         throw std::invalid_argument("Unknown primitive type: " + type);
      }

      auto primitive = std::make_shared<GeometricPrimitive>(shape, material);
      primitiveList->add(primitive);
      return primitiveList;
   }

   std::shared_ptr<PrimitiveList> PrimitiveFactory::create(
      const ParamSet& params, const Scene& scene
   ) {
      if (!params.has("type")) {
         throw std::invalid_argument("ObjectFactory requires a 'type' parameter");
      }

      auto type = params.retrieve<std::string>("type");
      if (isGeometricPrimitive(type)) {
         return createGeometricPrimitive(params, scene);
      } else {
         throw std::invalid_argument("Invalid primitive 'type', received: " + type);
      }
   }
}