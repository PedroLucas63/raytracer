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

         virtual RGBColor getColor(const Point3& point) const = 0;
         virtual const std::string& getName() const { return _name; }
         virtual bool isAnonymous() const { return _annonymous; }
   };
}

#endif // !OBJECT_MATERIAL_HPP