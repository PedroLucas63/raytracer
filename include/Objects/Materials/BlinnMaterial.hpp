#ifndef BLINN_MATERIAL_HPP
#define BLINN_MATERIAL_HPP

#include "Objects/Materials/Material.hpp"
#include "Image/RGBColor.hpp"
#include "Math/Vector3.hpp"
#include "Math/Point3.hpp"
#include "Objects/Surfel.hpp"
#include "Scene/Scene.hpp"

namespace raytracer {
   class BlinnMaterial : public Material {
      private:
         RGBColor _diffuse;
         RGBColor _specular;
         RGBColor _ambient;
         float    _glossiness;

         void lambertianReflection(
            const Vector3&                normal,
            const Vector3&                lightDir,
            const std::shared_ptr<Light>& light,
            float                         att,
            RGBColor&                     L
         ) const;

         Vector3 computeLightDirection(
            const Point3&              surfelPoint,
            const std::shared_ptr<Light> light
         ) const;

         void specularReflection(
            const Vector3&                viewDir,
            const Vector3&                normal,
            const Vector3&                lightDir,
            const std::shared_ptr<Light>& light,
            float                         att,
            RGBColor&                     L
         ) const;

         bool isOccluded(
            const Point3&                 hitPoint,
            const Vector3&                normal,
            const Vector3&                lightDir,
            const std::shared_ptr<Light>& light,
            const Scene&                  scene
         ) const;

         void ambientContribution(const Scene& scene, RGBColor& L) const;

         Vector3 computeHalfVector(
            const Vector3& viewDir,
            const Vector3& lightDir
         ) const;

      public:
         BlinnMaterial(const ParamSet& params);
         RGBColor getColor(const Point3& point) const override;
         RGBColor getColor(const Surfel& surfel, const Scene& scene) const;

         RGBColor getDiffuse()  const;
         RGBColor getSpecular() const;
         RGBColor getAmbient()  const;
         float    getGlossiness() const;

         void setDiffuse (const Vector3& diffuse);
         void setSpecular(const Vector3& specular);
         void setAmbient (const Vector3& ambient);
         void setGlossiness(float glossiness);
   };
}

#endif // !BLINN_MATERIAL_HPP