#include "Integrator/RayCastIntegrator.hpp"
#include "Objects/Aggregate/AggregatePrimitive.hpp"

namespace raytracer {
   std::optional<RGBColor> RayCastIntegrator::Li(const Ray& ray, const Scene& scene) const {
      std::optional<RGBColor> color = std::nullopt;
      
      std::vector<Surfel> intersections;
      for (const auto& primitive : scene.getPrimitives()) {
         if (!primitive) continue;

         Surfel sf(0.0f, Point3(), nullptr);

         auto isAggregate = std::dynamic_pointer_cast<AggregatePrimitive>(primitive) != nullptr;
         if (!isAggregate && !primitive->hasMaterial())
            continue;
         
         if (primitive->intersectWithSurfel(ray, &sf)) {
            intersections.push_back(sf);
         }
      }

      if (intersections.size() > 0) {
         auto closestSurfel = *std::min_element(
            intersections.begin(), intersections.end(),
            [](const Surfel& a, const Surfel& b) {
               return a.t < b.t;
            }
         );
         
         auto p = closestSurfel.point;
         color = closestSurfel.material->getColor(p);
      }

      return color;
   }
}