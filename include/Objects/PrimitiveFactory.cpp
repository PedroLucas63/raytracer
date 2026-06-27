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
      const ParamSet& params, const GraphicsState& graphicsState
   ) {
      std::shared_ptr<Material> material;

      if (!params.has("material")) {
         material = graphicsState.getActivatedMaterial();
      } else {
         std::string material_name = params.retrieve<std::string>("material");
         material = graphicsState.getMaterialAt(material_name);
      }

      if (material == nullptr)
         std::cerr << "[WARN]: Primitive created without material" << std::endl;

      auto primitiveList = std::make_shared<PrimitiveList>();
      std::string type = params.retrieve<std::string>("type");
      std::shared_ptr<Shape> shape;

      bool flipNormals;
      std::shared_ptr<Transform> transform;
      Api::getCurrentTransform(&transform, &flipNormals);
      
      static const auto identityTransform = std::make_shared<Transform>();

      if (type == "sphere") {
         shape = std::make_shared<Sphere>();
      } else if (type == "plane") {
         shape = std::make_shared<Plane>();
      } else if(type == "box"){
         shape = std::make_shared<Box>();
      } else if (type == "cylinder") {
         shape = std::make_shared<Cylinder>();
      } else if (type == "trianglemesh") {
         auto shapes = std::make_shared<TriangleMesh>(params);
         auto triangles = shapes->makeTriangules(flipNormals);

         for (auto& shape : triangles) {
            auto p = std::make_shared<GeometricPrimitive>(shape, material);
            primitiveList->add(
               {p, identityTransform}
            );
         }

         return primitiveList;
      } else {
         throw std::invalid_argument("Unknown primitive type: " + type);
      }

      auto primitive = std::make_shared<GeometricPrimitive>(shape, material);
      primitiveList->add({primitive, identityTransform});
      return primitiveList;
   }

   std::shared_ptr<PrimitiveList> PrimitiveFactory::create(
      const ParamSet& params, const GraphicsState& graphicsState
   ) {
      if (!params.has("type")) {
         throw std::invalid_argument("ObjectFactory requires a 'type' parameter");
      }

      auto type = params.retrieve<std::string>("type");
      if (isGeometricPrimitive(type)) {
         return createGeometricPrimitive(params, graphicsState);
      } else {
         throw std::invalid_argument("Invalid primitive 'type', received: " + type);
      }
   }
}