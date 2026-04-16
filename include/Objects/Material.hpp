#ifndef OBJECT_MATERIAL_HPP
#define OBJECT_MATERIAL_HPP

#include "Image/RGBColor.hpp"
#include "Core/ParamSet.hpp"
#include "Math/Point3.hpp"
#include <string>

namespace raytracer {
   class Material {
      protected:
         std::string _name;
         bool _annonymous = false;

      public:
         Material(const std::string& name, bool anonymous = false): 
            _name(name), _annonymous(anonymous) {}
         Material(const ParamSet& params);
         ~Material() = default;

         virtual const RGBColor& getColor(const Point3& point) const = 0;
         virtual const std::string& getName() const { return _name; }
         virtual bool isAnonymous() const { return _annonymous; }
   };

   class FlatMaterial : public Material {
      private:
         RGBColor _color;

      public:
         FlatMaterial(const std::string& name,const RGBColor& color)
            :  Material(name), _color(color) {}
         FlatMaterial(const ParamSet& params);
         const RGBColor& getColor(const Point3& point) const;
   };

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
         const RGBColor& getColor(const Point3& point) const;
   };
}

#endif // !OBJECT_MATERIAL_HPP