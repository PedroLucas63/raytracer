#include "Objects/Materials/BlinnMaterial.hpp"
#include "Utils/Utils.hpp"
#include "Objects/Light/DirectionalLight.hpp"
#include "Objects/Light/PointLight.hpp"

namespace raytracer {
   BlinnMaterial::BlinnMaterial(const ParamSet& params) : Material(params) {
      if (params.has("diffuse")) {
         setDiffuse(params.retrieve<Vector3>("diffuse"));
      } else {
         throw std::invalid_argument("BlinnMaterial requires a 'diffuse' parameter of type Vector3");
      }

      if (params.has("specular")) {
         setSpecular(params.retrieve<Vector3>("specular"));
      } else {
         throw std::invalid_argument("BlinnMaterial requires a 'specular' parameter of type Vector3");
      }

      if (params.has("ambient")) {
         setAmbient(params.retrieve<Vector3>("ambient"));
      } else {
         throw std::invalid_argument("BlinnMaterial requires an 'ambient' parameter of type Vector3");
      }

      if (params.has("glossiness")) {
         _glossiness = params.retrieve<float>("glossiness");
      } else {
         throw std::invalid_argument("BlinnMaterial requires a 'glossiness' parameter of type float");
      }
   }

   Vector3 BlinnMaterial::getDiffuse() const {
      return _diffuse;
   }

   Vector3 BlinnMaterial::getSpecular() const {
      return _specular;
   }

   Vector3 BlinnMaterial::getAmbient() const {
      return _ambient;
   }

   float BlinnMaterial::getGlossiness() const {
      return _glossiness;
   }

   void BlinnMaterial::setDiffuse(const Vector3& diffuse) {
      _diffuse = diffuse.clamp(0.0, 1.0);
   }

   void BlinnMaterial::setSpecular(const Vector3& specular) {
      _specular = specular.clamp(0.0, 1.0);
   }

   void BlinnMaterial::setAmbient(const Vector3& ambient) {
      _ambient = ambient.clamp(0.0, 1.0);
   }

   void BlinnMaterial::setGlossiness(float glossiness) {
      _glossiness = glossiness;
   }

   RGBColor BlinnMaterial::getColor(const Point3& point) const {
      return RGBColor::fromNormalized(
         _diffuse.getX(), 
         _diffuse.getY(), 
         _diffuse.getZ()
      );
   }

   RGBColor BlinnMaterial::getColor(const Surfel& surfel, const Scene& scene) const {
      RGBColor L;

      for (auto& light : scene.getLights()) {
         lambertianReflection(surfel.point, surfel.normal, light, &L);
         specularReflection();
      }

      ambientContribution(scene, &L);

      return L;
   }

   void BlinnMaterial::lambertianReflection(
      const Point3& surfelPoint,
      const Vector3& normal,
      const std::shared_ptr<Light>& light,
      RGBColor* L
   ) const {
      auto lightDir = computeLightDirection(surfelPoint, light);
      auto NdotL = std::max(0.0, normal.dot(lightDir));
      auto lightColor = light->getIntensity();
      auto color = multiplyColorByIntensity(lightColor, _diffuse);
      *L += color * NdotL;
   }

   void BlinnMaterial::specularReflection() const {
      
   }

   void BlinnMaterial::ambientContribution(const Scene& scene, RGBColor* L) const {
      if (auto light = scene.getAmbientLight()) {
         auto ambientColor = light->getIntensity();
         auto color = multiplyColorByIntensity(ambientColor, _ambient);
         *L += color;
      }
   }

   Vector3 BlinnMaterial::computeLightDirection(
      const Point3& surfelPoint, 
      const std::shared_ptr<Light> light
   ) const {
      auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light);
         return directionalLight->getDirection();
      
      auto pointLight = std::dynamic_pointer_cast<PointLight>(light);
         return (pointLight->getPosition() - surfelPoint).normalize();

      return Vector3(0, 0, 0);
   }
}