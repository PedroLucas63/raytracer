#ifndef TOON_INTEGRATOR_HPP
#define TOON_INTEGRATOR_HPP

#include "Integrator/Integrator.hpp"
#include <vector>

namespace raytracer {
   class ToonIntegrator : public SamplerIntegrator {
      private:
         std::vector<float> _intervals;
         static const uint MAX_NUMBER_OF_INTERVALS = 15;

      public:
         ToonIntegrator();
         ToonIntegrator(const std::vector<float>& intervals);
         ToonIntegrator(const ParamSet& params);
         ~ToonIntegrator() = default;

         std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const;

         std::vector<float> getIntervals() const;
         void setIntervals(const std::vector<float>& intervals);
         void generateIntervals(const uint numIntervals);
   };
}

#endif // TOON_INTEGRATOR_HPP