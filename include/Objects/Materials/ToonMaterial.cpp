#include "Objects/Materials/ToonMaterial.hpp"
#include "Objects/Light/PointLight.hpp"
#include "Objects/Light/DirectionalLight.hpp"

namespace raytracer {
   ToonMaterial::ToonMaterial(
      const std::vector<RGBColor>& intervalColors,
      const std::string& name, 
      bool anonymous = false
   ) : _intervalColors(intervalColors), Material(name, anonymous) {}

   ToonMaterial::ToonMaterial(const ParamSet& params) : Material(params) {
      if (params.has("color_map")) {
         auto colors = params.retrieve<std::vector<RGBColor>>("color_map");
         setIntervalColors(colors);
      } else {
         throw std::invalid_argument("ToonMaterial requires a 'color_map' parameter of type vector of RGBColor");
      }
   }
         
   std::vector<RGBColor> ToonMaterial::getIntervalColors() const {
      return _intervalColors;
   }

   uint ToonMaterial::getNumberOfColors() const {
      return _intervalColors.size();
   }


   void ToonMaterial::setIntervalColors(const std::vector<RGBColor>& intervalColors) {
      if (intervalColors.empty())
         throw std::invalid_argument("Interval colors cannot be empty.");
      _intervalColors = std::vector<RGBColor>(intervalColors.rbegin(), intervalColors.rend());
   }

   RGBColor ToonMaterial::getColor(const Point3& point) const {
      return _intervalColors.front();
   }

   RGBColor ToonMaterial::getColor(
      const Surfel& surfel, 
      const Scene& scene, 
      const std::vector<float> intervals
   ) const {
      Vector3 accColor(0.0f, 0.0f, 0.0f);
      int lights = 0;

      // Get angle by normal and light
      for (auto& light : scene.getLights()) {
         auto lightDir = getLightDirection(light, surfel.point);
         auto angle = getAngleByLight(lightDir, surfel.normal);
         auto index = getColorIndex(angle, intervals);

         if (index != -1) {
            auto color = _intervalColors.at(index);
            accColor += Vector3(
               color.getRedNormalized(), color.getGreenNormalized(), color.getBlueNormalized()
            );
            lights++;
         }
      }
      
      RGBColor L = RGBColor::fromNormalized(
         accColor.getX() / lights, 
         accColor.getY() / lights, 
         accColor.getZ() / lights
      );

      return L;
   }

   Vector3 ToonMaterial::getLightDirection(
      const std::shared_ptr<Light>& light, const Point3& point
   ) const {
      if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light)) {
         return (pointLight->getPosition() - point).normalize();
      } else if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
         return directionalLight->getDirection();
      } else {
         throw std::invalid_argument("Unsupported light type for ToonMaterial");
      }
   }

   float ToonMaterial::getAngleByLight(
      const Vector3& lightDir, const Vector3& normal
   ) const {
      return normal.angleBetween(lightDir);
   }

    uint ToonMaterial::getColorIndex(
      float angle, const std::vector<float> intervals
   ) const {
      if (angle >= 90) return -1;

      auto index = intervals.size();
      for (int i = 0; i < intervals.size(); i++) {
         if (angle < intervals.at(i)) {
            index = i;
            break;
         }
      }

      if (index > _intervalColors.size() - 1) 
         index = _intervalColors.size() - 1;
      
      return index;
   }
}