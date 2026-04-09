#include "Material.hpp"

namespace raytracer {
   Material::Material(const ParamSet& params) {
      if (!params.has("name")) {
         throw std::invalid_argument("Material requires a 'name' parameter");
      }
      _name = params.retrieve<std::string>("name");
   }

   ColorMaterial::ColorMaterial(const ParamSet& params) : Material(params) {
      if (!params.has("color")) {
         throw std::invalid_argument("ColorMaterial requires a 'color' parameter");
      }
      _color = params.retrieveOrDefault("color", RGBColor(255, 255, 255));
   }

   const RGBColor& ColorMaterial::getColor() const {
      return _color;
   }
}