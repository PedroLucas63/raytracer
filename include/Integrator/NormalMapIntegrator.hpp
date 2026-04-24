#ifndef NORMAL_MAP_INTEGRATOR_HPP
#define NORMAL_MAP_INTEGRATOR_HPP

#include "Integrator/Integrator.hpp"

namespace raytracer {
    class NormalMapIntegrator : public SamplerIntegrator {
        public:
            NormalMapIntegrator() = default;
            ~NormalMapIntegrator() = default;

            std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const override;
    };
}

#endif // !NORMAL_MAP_INTEGRATOR_HPP