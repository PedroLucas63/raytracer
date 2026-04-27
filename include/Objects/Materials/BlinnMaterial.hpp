#ifndef BLINN_MATERIAL_HPP
#define BLINN_MATERIAL_HPP

#include "Objects/Materials/Material.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   class BlinnMaterial : public Material {
      private:
         Vector3 _diffuse;
         Vector3 _specular;
         Vector3 _ambient;
         float _glossiness;

      public:
         BlinnMaterial(const ParamSet& params);
         const RGBColor& getColor(const Point3& point) const override;

         Vector3 getDiffuse() const;
         Vector3 getSpecular() const;
         Vector3 getAmbient() const;
         float getGlossiness() const;

         void setDiffuse(const Vector3& diffuse);
         void setSpecular(const Vector3& specular);
         void setAmbient(const Vector3& ambient);
         void setGlossiness(float glossiness);
   };
}

#endif // !BLINN_MATERIAL_HPP