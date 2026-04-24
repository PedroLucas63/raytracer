#ifndef DEPTH_MAP_INTEGRATOR_HPP
#define DEPTH_MAP_INTEGRATOR_HPP

#include "Integrator/Integrator.hpp"

namespace raytracer {
    class DepthMapIntegrator : public SamplerIntegrator {
        private:
            RGBColor _nearColor;
            RGBColor _farColor;
            float _zmin;
            float _zmax;

        public:
            DepthMapIntegrator(
                const RGBColor& nearColor = RGBColor(255, 255, 255),
                const RGBColor& farColor  = RGBColor(0, 0, 0),
                float zmin = 0.0f,
                float zmax = std::numeric_limits<float>::max()
            ) : _nearColor(nearColor), _farColor(farColor), _zmin(zmin), _zmax(zmax) {}

            ~DepthMapIntegrator() noexcept override = default;

            void preprocess(const Scene& scene) override;

            std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const override;

        private:
            RGBColor lerp(float t) const;
    };
}

#endif // !DEPTH_MAP_INTEGRATOR_HPP