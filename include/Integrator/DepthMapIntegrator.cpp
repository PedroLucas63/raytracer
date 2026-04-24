#include "Integrator/DepthMapIntegrator.hpp"
#include <limits>
#include <algorithm>

namespace raytracer {

    void DepthMapIntegrator::preprocess(const Scene& scene) {
        // First: create camera (base class preprocess sets _camera)
        SamplerIntegrator::preprocess(scene);

        auto& film = _camera->film();
        int w = film->getWidth();
        int h = film->getHeight();

        float foundMin = std::numeric_limits<float>::max();
        float foundMax = -std::numeric_limits<float>::max();

        bool anyHit = false;

        for (int j = 0; j < h; ++j) {
            for (int i = 0; i < w; ++i) {
                Ray ray = _camera->generate_ray(i, j);
                Surfel sf;
                if (scene.intersect(ray, &sf)) {
                    anyHit = true;
                    foundMin = std::min(foundMin, sf.t);
                    foundMax = std::max(foundMax, sf.t);
                }
            }
        }

        // Only override defaults if we actually found hits
        if (anyHit) {
            // If the user didn't supply explicit zmin/zmax (they're at sentinel values), use scene values
            if (_zmin == 0.0f)
                _zmin = foundMin;
            if (_zmax == std::numeric_limits<float>::max())
                _zmax = foundMax;
        }
    }

    std::optional<RGBColor> DepthMapIntegrator::Li(const Ray& ray, const Scene& scene) const {
        Surfel surfel;
        if (!scene.intersect(ray, &surfel))
            return std::nullopt;

        // Normalize t into [0,1] within [_zmin, _zmax]
        float range = _zmax - _zmin;
        float t = (range > 0.0f) ? (surfel.t - _zmin) / range : 0.0f;
        t = std::clamp(t, 0.0f, 1.0f);

        return lerp(t);
    }

    RGBColor DepthMapIntegrator::lerp(float t) const {
        // t=0 → near (close), t=1 → far
        float r = _nearColor.getRed()   + t * (static_cast<float>(_farColor.getRed())   - _nearColor.getRed());
        float g = _nearColor.getGreen() + t * (static_cast<float>(_farColor.getGreen()) - _nearColor.getGreen());
        float b = _nearColor.getBlue()  + t * (static_cast<float>(_farColor.getBlue())  - _nearColor.getBlue());
        return RGBColor(
            static_cast<uint8_t>(std::clamp(r, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(g, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(b, 0.0f, 255.0f))
        );
    }

}