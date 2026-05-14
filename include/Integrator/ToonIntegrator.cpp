#include "Integrator/ToonIntegrator.hpp"
#include "Objects/Materials/ToonMaterial.hpp"

namespace raytracer {
   ToonIntegrator::ToonIntegrator() : _intervals() {}

   ToonIntegrator::ToonIntegrator(const std::vector<float>& intervals) {
      setIntervals(intervals);
   }

   ToonIntegrator::ToonIntegrator(const ParamSet& params) {
      if (params.has("mapping_interval")) {
         auto intervals = params.retrieve<std::vector<float>>("mapping_interval");
         setIntervals(intervals);
      } else if (params.has("n_intervals")) {
         auto numIntervals = params.retrieve<uint>("n_intervals");
         generateIntervals(numIntervals);
      } else {
         throw std::invalid_argument("ToonIntegrator requires a 'mapping_interval' or 'n_intervals' parameter.");
      }
   }

   void ToonIntegrator::generateIntervals(const uint numIntervals) {
      if (numIntervals > MAX_NUMBER_OF_INTERVALS)
         throw std::invalid_argument("Number of intervals cannot be greater than " + MAX_NUMBER_OF_INTERVALS);

      auto increment = 90.0f / (numIntervals + 1);      
      auto end = 0.0f;

      std::vector<float> intervals;

      for (int i = 0; i < numIntervals; i++) {
         end += increment;
         intervals.push_back(increment);
      }

      setIntervals(intervals);
   }

   std::vector<float> ToonIntegrator::getIntervals() const {
      return _intervals;
   }

   void ToonIntegrator::setIntervals(const std::vector<float>& intervals) {
      if (intervals.size() > MAX_NUMBER_OF_INTERVALS)
         throw std::invalid_argument("Number of intervals cannot be greater than " + MAX_NUMBER_OF_INTERVALS);
      
      _intervals = intervals;
      std::sort(_intervals.begin(), _intervals.end());
   }

   std::optional<RGBColor> ToonIntegrator::Li(const Ray& ray, const Scene& scene) const {
      Surfel surfel;

      if (!scene.intersectWithSurfel(ray, &surfel))
         return std::nullopt;

      auto toonMaterial = std::dynamic_pointer_cast<ToonMaterial>(surfel.material);

      if (!toonMaterial)
         return std::nullopt;
      
      return toonMaterial->getColor(surfel, scene, _intervals);
   }
}