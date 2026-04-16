#include "Material.hpp"
#include <cmath>

namespace raytracer {
   Material::Material(const ParamSet& params) {
      bool anonymous = !params.has("name");
      
      std::string name;
      if (anonymous) {
         name = "__material_" + std::to_string(reinterpret_cast<std::uintptr_t>(this));
      } else {
         name = params.retrieve<std::string>("name");
      }

      if (name.starts_with("__") && !anonymous) {
         throw std::invalid_argument("Material name cannot start with '__' unless it's anonymous");
      }

      _name = name;
      _annonymous = anonymous;
   }

   FlatMaterial::FlatMaterial(const ParamSet& params) : Material(params) {
      if (!params.has("color")) {
         throw std::invalid_argument("FlatMaterial requires a 'color' parameter");
      }
      _color = params.retrieveOrDefault("color", RGBColor(255, 255, 255));
   }

   const RGBColor& FlatMaterial::getColor(const Point3& point) const {
      return _color;
   }

   GridMaterial::GridMaterial(const ParamSet& params) : Material(params) {
      _color1 = params.retrieveOrDefault("color1", RGBColor(255, 255, 255));
      _color2 = params.retrieveOrDefault("color2", RGBColor(123, 123, 123));
      _spacing = params.retrieveOrDefault("spacing", 1.0f);

      if (_spacing <= 0) throw std::invalid_argument("GridMaterial requires a 'spacing' positive");
   }

   const RGBColor& GridMaterial::getColor(const Point3& point) const {
      auto x_pos = static_cast<int>(std::floor(point.getX() / _spacing));
      auto y_pos = static_cast<int>(std::floor(point.getY() / _spacing));

      bool xEven = x_pos % 2 == 0;
      bool yEven = y_pos % 2 == 0;

      return (xEven ^ yEven) ? _color1 : _color2;
   }
}