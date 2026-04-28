#ifndef BLINN_PHONG_INTEGRATOR_HPP
#define BLINN_PHONG_INTEGRATOR_HPP

#include "Integrator/Integrator.hpp"
#include "Objects/Light/Light.hpp"
#include "Objects/Light/AmbientLight.hpp"
#include "Objects/Materials/BlinnMaterial.hpp"
#include "Core/ParamSet.hpp"

namespace raytracer {
   class BlinnPhongIntegrator : public SamplerIntegrator {
      private:
         uint _depth;   

         bool findClosestIntersection(
            const Ray& ray, 
            const Scene& scene, 
            Surfel* surfel
         ) const;

         std::shared_ptr<BlinnMaterial> getBlinnMaterial(const std::shared_ptr<Material>& material) const;

      public:
         BlinnPhongIntegrator();
         BlinnPhongIntegrator(uint depth);
         BlinnPhongIntegrator(const ParamSet& params);
         ~BlinnPhongIntegrator() = default;

         std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const override;
   };
}

#endif // !BLINN_PHONG_INTEGRATOR_HPP