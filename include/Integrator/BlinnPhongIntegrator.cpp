#include "Integrator/BlinnPhongIntegrator.hpp"
#include "Objects/Light/DirectionalLight.hpp"
#include "Objects/Light/PointLight.hpp"
#include "Objects/Materials/BlinnMaterial.hpp"

namespace raytracer {
   std::optional<RGBColor> BlinnPhongIntegrator::Li(const Ray& ray, const Scene& scene) const {
      Surfel surfel;
      if (!findClosestIntersection(ray, scene, &surfel))
         return std::nullopt;

      auto material = getBlinnMaterial(surfel.material);
      if (!material)
         return std::nullopt;

      RGBColor L;

      for (auto& light : scene.getLights()) {
         lambertianReflection(surfel.point, surfel.normal, light, material, &L);
         // TODO: Add the Specular Reflection
      }

      if (auto ambientLight = getAmbientLight(scene)) {
         auto ambientColor = ambientLight->getIntensity();
         auto ambientWeight = material->getAmbient();

         L += multiplyColorByIntensity(ambientColor, ambientWeight);
      }

      return L;
   }

   bool BlinnPhongIntegrator::findClosestIntersection(
      const Ray& ray, 
      const Scene& scene, 
      Surfel* surfel
   ) const {
      if (!scene.intersectWithSurfel(ray, surfel))
         return false;

      // TODO: [2]
      // [2] SPECIAL SITUATION: IF THE RAY HITS THE SURFACE FROM "BEHIND" (INSIDE), WE DO NOT COLOR.

      return true;
   }

   std::shared_ptr<BlinnMaterial> BlinnPhongIntegrator::getBlinnMaterial(
      const std::shared_ptr<Material>& material
   ) const {
      return std::dynamic_pointer_cast<BlinnMaterial>(material);
   }

   void BlinnPhongIntegrator::lambertianReflection(
      const Point3& surfelPoint,
      const Vector3& normal,
      const std::shared_ptr<Light>& light,
      const std::shared_ptr<BlinnMaterial>& material,
      RGBColor* L
   ) const {
      auto lightDir = computeLightDirection(surfelPoint, light);
      auto NdotL = std::max(0.0, normal.dot(lightDir));

      auto lightColor = light->getIntensity();
      auto diffuseWeight = material->getDiffuse();

      auto color = multiplyColorByIntensity(lightColor, diffuseWeight);

      *L += color * NdotL;
   }

   Vector3 BlinnPhongIntegrator::computeLightDirection(
      const Point3& surfelPoint, 
      const std::shared_ptr<Light> light
   ) const {
      auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light);
         return directionalLight->getDirection();
      
      auto pointLight = std::dynamic_pointer_cast<PointLight>(light);
         return (pointLight->getPosition() - surfelPoint).normalize();

      return Vector3(0, 0, 0);
   }

   RGBColor BlinnPhongIntegrator::multiplyColorByIntensity(
      const RGBColor& color, 
      const Vector3& intensity
   ) const {
      return color.fromNormalized(
         color.getRedNormalized() * intensity.getX(),
         color.getGreenNormalized() * intensity.getY(),
         color.getBlueNormalized() * intensity.getZ()
      );
   }

   std::shared_ptr<AmbientLight> BlinnPhongIntegrator::getAmbientLight(const Scene& scene) const {
      for (const auto& light : scene.getLights()) {
         auto ambientLight = std::dynamic_pointer_cast<AmbientLight>(light);
         if (ambientLight)
            return ambientLight;
      }
      return nullptr;
   }
}