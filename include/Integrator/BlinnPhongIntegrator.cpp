#include "Integrator/BlinnPhongIntegrator.hpp"
#include "Objects/Materials/BlinnMaterial.hpp"

namespace raytracer {
   BlinnPhongIntegrator::BlinnPhongIntegrator() : _depth(0) {}
   BlinnPhongIntegrator::BlinnPhongIntegrator(uint depth) : _depth(depth) {}
   BlinnPhongIntegrator::BlinnPhongIntegrator(const ParamSet& params) {
      _depth = params.retrieveOrDefault<uint>("depth", 0);
   }

   std::optional<RGBColor> BlinnPhongIntegrator::Li(const Ray& ray, const Scene& scene) const {
      Surfel surfel;
      if (!findClosestIntersection(ray, scene, &surfel))
         return std::nullopt;

      auto material = getBlinnMaterial(surfel.material);
      if (!material)
         return surfel.material->getColor(surfel.point);

      return material->getColor(surfel, scene);
   }

   bool BlinnPhongIntegrator::findClosestIntersection(
      const Ray& ray, 
      const Scene& scene, 
      Surfel* surfel
   ) const {
      if (!scene.intersectWithSurfel(ray, surfel))
         return false;

      if(ray.direction.dot(surfel->normal) > 0) {
         return false;
      }

      return true;
   }

   std::shared_ptr<BlinnMaterial> BlinnPhongIntegrator::getBlinnMaterial(
      const std::shared_ptr<Material>& material
   ) const {
      return std::dynamic_pointer_cast<BlinnMaterial>(material);
   }
}