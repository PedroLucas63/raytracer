#ifndef OBJECT_FLAT_MATERIAL_HPP
#define OBJECT_FLAT_MATERIAL_HPP

#include "Objects/Materials/Material.hpp"

namespace raytracer {
   class FlatMaterial : public Material {
      private:
         RGBColor _color;

      public:
         FlatMaterial(const std::string& name,const RGBColor& color)
            :  Material(name), _color(color) {}
         FlatMaterial(const ParamSet& params);
         const RGBColor& getColor(const Point3& point) const;
   };
}

#endif // !OBJECT_FLAT_MATERIAL_HPP