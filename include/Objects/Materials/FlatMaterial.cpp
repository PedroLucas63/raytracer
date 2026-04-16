#include "Objects/Materials/FlatMaterial.hpp"

namespace raytracer {
   FlatMaterial::FlatMaterial(const ParamSet& params) : Material(params) {
      if (!params.has("color")) {
         throw std::invalid_argument("FlatMaterial requires a 'color' parameter");
      }
      _color = params.retrieve<RGBColor>("color");
   }

   const RGBColor& FlatMaterial::getColor(const Point3& point) const {
      return _color;
   }
}