#ifndef BLINN_PHONG_INTEGRATOR_HPP
#define BLINN_PHONG_INTEGRATOR_HPP

#include "Integrator/Integrator.hpp"
#include "Objects/Light/Light.hpp"
#include "Objects/Light/AmbientLight.hpp"

namespace raytracer {
   class BlinnPhongIntegrator : public SamplerIntegrator {
      private:
         bool findClosestIntersection(
            const Ray& ray, 
            const Scene& scene, 
            Surfel* surfel
         ) const;

         std::shared_ptr<BlinnMaterial> getBlinnMaterial(const std::shared_ptr<Material>& material) const;

         void lambertianReflection(
            const Point3& surfelPoint,
            const Vector3& normal,
            const std::shared_ptr<Light>& light,
            const std::shared_ptr<BlinnMaterial>& material,
            RGBColor* L
         ) const;

         Vector3 computeLightDirection(
            const Point3& surfelPoint, 
            const std::shared_ptr<Light> light
         ) const;

         std::shared_ptr<AmbientLight> getAmbientLight(const Scene& scene) const;

         RGBColor multiplyColorByIntensity(const RGBColor& color, const Vector3& intensity) const;

      public:
         BlinnPhongIntegrator() = default;
         ~BlinnPhongIntegrator() = default;

         std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const override;
   };
}

#endif // !BLINN_PHONG_INTEGRATOR_HPP