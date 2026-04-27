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
                if (scene.intersectWithSurfel(ray, &sf)) {
                    anyHit = true;
                    foundMin = std::min(foundMin, sf.t);
                    foundMax = std::max(foundMax, sf.t);
                }
            }
        }
        // Only override defaults if we actually found hits
        if (anyHit) {
            float sceneRange = foundMax - foundMin;

            _realZmin = foundMin + _zmin * sceneRange;
            _realZmax = foundMin + _zmax * sceneRange;
        }
    }

    std::optional<RGBColor> DepthMapIntegrator::Li(const Ray& ray, const Scene& scene) const {
        Surfel surfel;
        if (!scene.intersectWithSurfel(ray, &surfel))
            return std::nullopt;

        // Normalize t into [0,1] within [_realZmin, _realZmax]
        float range = _realZmax - _realZmin;
        float t = (range > 0.0f) ? (surfel.t - _realZmin) / range : 0.0f;
        t = std::clamp(t, 0.0f, 1.0f);

        return RGBColor::lerp(_farColor, _nearColor, t);
    }

}