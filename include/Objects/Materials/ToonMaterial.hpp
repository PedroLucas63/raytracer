#ifndef TOON_MATERIAL_HPP
#define TOON_MATERIAL_HPP

#include "Objects/Materials/Material.hpp"
#include "Image/RGBColor.hpp"
#include "Objects/Surfel.hpp"
#include "Scene/Scene.hpp"
#include "Math/Vector3.hpp"
#include "Math/Point3.hpp"
#include <vector>
#include <memory>

namespace raytracer {
   class ToonMaterial : public Material {
      private:
         std::vector<RGBColor> _intervalColors;

         float getAngleByLight(const Vector3& lightDir, const Vector3& normal) const;
         Vector3 getLightDirection(
            const std::shared_ptr<Light>& light, const Point3& point
         ) const;
         uint getColorIndex(
            float angle, const std::vector<float> intervals
         ) const;

      public:
         ToonMaterial(
            const std::vector<RGBColor>& intervalColors,
            const std::string& name, 
            bool anonymous
         );
         ToonMaterial(const ParamSet& params);
         ~ToonMaterial() = default;
         
         std::vector<RGBColor> getIntervalColors() const;
         uint getNumberOfColors() const;
         void setIntervalColors(const std::vector<RGBColor>& intervalColors);

         RGBColor getColor(const Point3& point) const;
         RGBColor getColor(
            const Surfel& surfel, 
            const Scene& scene, 
            const std::vector<float> intervals
         ) const;
   };
}

#endif // !TOON_MATERIAL_HPP