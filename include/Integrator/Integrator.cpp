#include "Integrator/Integrator.hpp"
#include "Scene/Camera.hpp"
#include "Utils/ProgressBar.hpp"

namespace raytracer {
   ProgressBar SamplerIntegrator::makeProgressBar() {
      auto& film = _camera->film();
      int w = film.getWidth();
      int h = film.getHeight();

      ProgressBar progress(
         {
            {w, "Cols"},
            {h, "Rows"}
         }
      );

      progress.setTitle("Ray Tracing Scene").render();

      return progress;
   }

   std::tuple<uint, uint> SamplerIntegrator::getIAndJ(const ProgressBar::Iterator::reference& it) const {
      auto h = _camera->film().getHeight();

      auto col = it[0], row = it[1];
      auto i = col;
      auto j = h - 1 - row;

      return {i, j};
   }

   RGBColor SamplerIntegrator::sampleBackground(const Scene& scene, uint i, uint j) const {
      auto background = scene.getBackground();
      auto& film = _camera->film();

      float u_norm = static_cast<float>(i) / (film.getWidth() - 1);
      float v_norm = static_cast<float>(j) / (film.getHeight() - 1);

      return background->sampleUV(u_norm, v_norm);
   }

   void SamplerIntegrator::render(const Scene& scene) {
      preprocess(scene);

      auto progress = makeProgressBar();
      auto film = _camera->film();

      for (auto it : progress) {
         auto [i, j] = getIAndJ(it);
         Ray ray = _camera->generate_ray(i, j);

         auto tempColor = Li(ray, scene);
         auto color = tempColor.has_value() ? tempColor.value() : sampleBackground(scene, i, j);

         film.setPixel(color, j, i);
      }
   }

   void SamplerIntegrator::preprocess(const Scene& scene) {
      _camera = CameraFactory::create(scene.getParams());
   }

   void SamplerIntegrator::saveImage() const {
      _camera->film().save();
   }
}