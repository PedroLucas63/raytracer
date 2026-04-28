#include "Utils/Utils.hpp"

namespace raytracer {
   RGBColor multiplyColorByIntensity(const RGBColor& color, const Vector3& intensity) {
      return RGBColor::fromNormalized(
         color.getRedNormalized() * intensity.getX(),
         color.getGreenNormalized() * intensity.getY(),
         color.getBlueNormalized() * intensity.getZ()
      );
   }
}