#ifndef INTEGRATOR_HPP
#define INTEGRATOR_HPP

#include "Scene/Camera.hpp"
#include "Image/RGBColor.hpp"
#include "Scene/Scene.hpp"
#include "Math/Ray.hpp"
#include <memory>
#include <optional>

namespace raytracer {
   class Integrator {
      public:
         virtual ~Integrator() = default;
         virtual void render(const Scene& scene) = 0;
   };

   class SamplerIntegrator : public Integrator {
      protected:
		   std::shared_ptr<const Camera> _camera;

         ProgressBar makeProgressBar();
         std::tuple<uint, uint> getIAndJ(const ProgressBar::Iterator::reference& it) const;
         RGBColor sampleBackground(const Scene& scene, uint i, uint j) const;

      public:
         SamplerIntegrator() = default;
         virtual ~SamplerIntegrator() = default;

         virtual std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const = 0;
         virtual void render(const Scene& scene);
         virtual void preprocess(const Scene& scene);
         virtual void saveImage() const;
   };
}

#endif // !INTEGRATOR_HPP