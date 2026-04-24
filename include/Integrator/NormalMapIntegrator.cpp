#include "Integrator/NormalMapIntegrator.hpp"
#include <algorithm>

namespace raytracer {

   std::optional<RGBColor> NormalMapIntegrator::Li(const Ray& ray, const Scene& scene) const {
      Surfel surfel;
      if (!scene.intersect(ray, &surfel))
         return std::nullopt;

      // Normal is already unit-length from the shape's intersectWithSurfel.
      // Map from [-1, 1] to [0, 255]:
      //   1. shift: n + 1  → [0, 2]
      //   2. scale: / 2    → [0, 1]
      //   3. scale: * 255  → [0, 255]
      auto n = surfel.normal.normalize();

      auto toChannel = [](float v) -> uint8_t {
         float mapped = (v + 1.0f) / 2.0f * 255.0f;
         return static_cast<uint8_t>(std::clamp(mapped, 0.0f, 255.0f));
      };

      return RGBColor(
         toChannel(static_cast<float>(n.getX())),
         toChannel(static_cast<float>(n.getY())),
         toChannel(static_cast<float>(n.getZ()))
      );
   }

}