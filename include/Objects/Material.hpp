#ifndef OBJECT_MATERIAL_HPP
#define OBJECT_MATERIAL_HPP

#include "Image/RGBColor.hpp"
#include "Core/ParamSet.hpp"
#include <string>

namespace raytracer {
   class Material {
      protected:
         std::string _name;

      public:
         Material(const std::string& name) : _name(name) {};
         Material(const ParamSet& params);
         ~Material() = default;

         virtual const RGBColor& getColor() const = 0;
         virtual const std::string& getName() const { return _name; }
   };

   class ColorMaterial : public Material {
      private:
         RGBColor _color;

      public:
         ColorMaterial(const std::string& name,const RGBColor& color)
            :  Material(name), _color(color) {}
         ColorMaterial(const ParamSet& params);
         const RGBColor& getColor() const;
   };
}

#endif // !OBJECT_MATERIAL_HPP