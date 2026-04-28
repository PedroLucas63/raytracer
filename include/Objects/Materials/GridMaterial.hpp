#ifndef OBJECT_GRID_MATERIAL_HPP
#define OBJECT_GRID_MATERIAL_HPP

#include "Objects/Materials/Material.hpp"

namespace raytracer {
   class GridMaterial : public Material {
      private:
         RGBColor _color1, _color2;
         float _spacing;

      public:
         GridMaterial(
            const std::string& name, 
            const RGBColor& color1, const RGBColor& color2,
            float spacing
         )
         :  Material(name), _color1(color1), _color2(color2), _spacing(spacing) {}
         GridMaterial(const ParamSet& params);
         RGBColor getColor(const Point3& point) const override;
   };
}

#endif // !OBJECT_GRID_MATERIAL_HPP