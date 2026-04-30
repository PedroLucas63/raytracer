#include "Objects/Materials/BlinnMaterial.hpp"
#include "Utils/Utils.hpp"

#include "Scene/Background/Background.hpp"
#include "Scene/Background/BackgroundImage.hpp"
#include "Objects/Light/DirectionalLight.hpp"
#include "Objects/Light/PointLight.hpp"
#include "Math/Ray.hpp"
#include <cmath>
#include <limits>

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

      _reflectivity = params.retrieveOrDefault<float>("reflectivity", 0.0f);
   }

   // --- Getters ---

   RGBColor BlinnMaterial::getDiffuse()  const { return _diffuse;  }
   RGBColor BlinnMaterial::getSpecular() const { return _specular; }
   RGBColor BlinnMaterial::getAmbient()  const { return _ambient;  }
   float    BlinnMaterial::getGlossiness() const { return _glossiness; }
   float    BlinnMaterial::getReflectivity() const { return _reflectivity; }

   // --- Setters (receive Vector3 from XML parser, convert to RGBColor) ---

   void BlinnMaterial::setDiffuse(const Vector3& diffuse) {
      _diffuse = RGBColor::fromNormalized(
         static_cast<float>(diffuse.getX()),
         static_cast<float>(diffuse.getY()),
         static_cast<float>(diffuse.getZ())
      );
   }

   void BlinnMaterial::setSpecular(const Vector3& specular) {
      _specular = RGBColor::fromNormalized(
         static_cast<float>(specular.getX()),
         static_cast<float>(specular.getY()),
         static_cast<float>(specular.getZ())
      );
   }

   void BlinnMaterial::setAmbient(const Vector3& ambient) {
      _ambient = RGBColor::fromNormalized(
         static_cast<float>(ambient.getX()),
         static_cast<float>(ambient.getY()),
         static_cast<float>(ambient.getZ())
      );
   }

   void BlinnMaterial::setGlossiness(float glossiness) {
      _glossiness = glossiness;
   }

   void BlinnMaterial::setReflectivity(float reflectivity) {
      _reflectivity = std::clamp(reflectivity, 0.0f, 1.0f);
   }

   // --- Color ---

   RGBColor BlinnMaterial::getColor(const Point3& point) const {
      return _diffuse;
   }

   RGBColor BlinnMaterial::getColor(
      const Surfel& surfel, 
      const Scene& scene,
      const int currentDepth = 0,
      const int maxDepth = 0
   ) const {
      RGBColor L;

      for (auto& light : scene.getLights()) {
         auto lightDir = computeLightDirection(surfel.point, light);

         float att = 1.0f;
         if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light)) {
            auto distance = (pointLight->getPosition() - surfel.point).length();
            att = pointLight->computeAttenuation(distance);
         }

         if (isOccluded(surfel.point, surfel.normal, lightDir, light, scene))
            continue;


         lambertianReflection(surfel.normal, lightDir, light, att, L);
         specularReflection(surfel.viewDir, surfel.normal, lightDir, light, att, L);
      }

      ambientContribution(scene, L);

      if (_reflectivity > 0.0f && currentDepth < maxDepth) {
         auto reflectivityContribution = getReflectivyContribution(
            surfel,
            scene,
            currentDepth, 
            maxDepth
         );
         L = L * (1.0f - _reflectivity) + reflectivityContribution * _reflectivity;
      }

      return L;
   }

   // --- Shadow test ---

   bool BlinnMaterial::isOccluded(
      const Point3&                 hitPoint,
      const Vector3&                normal,
      const Vector3&                lightDir,
      const std::shared_ptr<Light>& light,
      const Scene&                  scene
   ) const {
      constexpr double SHADOW_EPS = 1e-4;

      double tMax = std::numeric_limits<double>::infinity();
      if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light)) {
         tMax = (pointLight->getPosition() - hitPoint).length() - SHADOW_EPS;
      }

      Point3 shadowOrigin = hitPoint + normal * SHADOW_EPS;
      Ray    shadowRay(shadowOrigin, lightDir, SHADOW_EPS, tMax);

      return scene.intersect(shadowRay);
   }

   // --- Shading ---

   void BlinnMaterial::lambertianReflection(
      const Vector3&                normal,
      const Vector3&                lightDir,
      const std::shared_ptr<Light>& light,
      float                         att,
      RGBColor&                     L
   ) const {
      auto NdotL = std::max(0.0, normal.dot(lightDir));
      if (NdotL == 0.0) return;

      auto lightColor = light->getIntensity();
      float factor = static_cast<float>(NdotL) * att;

      auto color = multiplyColorByIntensity(_diffuse, lightColor);
      L += color * factor;
   }

   void BlinnMaterial::specularReflection(
      const Vector3&                viewDir,
      const Vector3&                normal,
      const Vector3&                lightDir,
      const std::shared_ptr<Light>& light,
      float                         att,
      RGBColor&                     L
   ) const {
      auto NdotL = normal.dot(lightDir);
      if (NdotL <= 0) return;

      auto h = computeHalfVector(viewDir, lightDir);
      auto NdotH = std::max(0.0, normal.dot(h));

      auto specularFactor = std::pow(NdotH, _glossiness);
      if (specularFactor == 0.0) return;

      auto lightColor = light->getIntensity();
      float factor = static_cast<float>(specularFactor) * att;

      auto color = multiplyColorByIntensity(_specular, lightColor);
      L += color * factor;
   }

   void BlinnMaterial::ambientContribution(const Scene& scene, RGBColor& L) const {
      if (auto light = scene.getAmbientLight()) {
         auto ambientColor = light->getIntensity();

         auto color = multiplyColorByIntensity(_ambient, ambientColor);
         L += color;
      }
   }

   Vector3 BlinnMaterial::computeLightDirection(
      const Point3& surfelPoint,
      const std::shared_ptr<Light> light
   ) const {
      if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light))
         return -directionalLight->getDirection();

      if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light))
         return (pointLight->getPosition() - surfelPoint).normalize();

      return Vector3(0, 0, 0);
   }

   RGBColor BlinnMaterial::getReflectivyContribution(
      const Surfel& surfel,
      const Scene& scene,
      const int currentDepth, 
      const int maxDepth
   ) const {
      // [1] Get reflection direction;
      auto I = -surfel.viewDir;
      auto N = surfel.normal;
      auto reflectionDir = (I - N * 2 * (I.dot(N))).normalize();
      auto origin = surfel.point + N * 1e-4; // Offset to avoid self-intersection
      auto reflectionRay = Ray(origin, reflectionDir);

      // [2] Intersect with scene:
      Surfel reflectionSurfel;
      if (scene.intersectWithSurfel(reflectionRay, &reflectionSurfel)) {
         // [2.1] Has intersection? Call getColor (check if is BlinnMaterial or not);
         auto material = reflectionSurfel.material;

         if (auto blinnMaterial = std::dynamic_pointer_cast<BlinnMaterial>(material)) {
            return blinnMaterial->getColor(reflectionSurfel, scene, currentDepth + 1, maxDepth);
         }

         return material->getColor(reflectionSurfel.point);
      } else {
         // [2.2] Dont has intersection? Get background color.
         if (!scene.hasBackground()) {
            return RGBColor(0, 0, 0); // Default to black if no background is set
         }

         auto background = scene.getBackground();
         auto imageBg = std::dynamic_pointer_cast<BackgroundImage>(background);
         if (imageBg && imageBg->isSpherical())
            return imageBg->sampleDirection(reflectionDir);

         return RGBColor(0, 0, 0); // Default to black if no spherical background is set
      }
      // [3] Return the color
   }

   Vector3 BlinnMaterial::computeHalfVector(
      const Vector3& viewDir,
      const Vector3& lightDir
   ) const {
      auto numerator   = viewDir + lightDir;
      auto denominator = numerator.length();

      if (denominator < 1e-8)
         return Vector3(0, 0, 0);
      return numerator / denominator;
   }
}