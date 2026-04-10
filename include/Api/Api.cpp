#include "Api.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include "Utils/ProgressBar.hpp"
#include <iostream>
#include "Objects/Shapes/Sphere.hpp"
#include "Objects/Surfel.hpp"

namespace raytracer {
   // Define the static member variable
   RunningOptions Api::_options;
   Scene Api::_scene;

   void Api::render() {
      const auto& params = _scene.getParams();

      auto camera = CameraFactory::build(params); 
      auto background = BackgroundFactory::build(params);
      auto film = camera->film();

      int w = film.getWidth();
      int h = film.getHeight();

      ProgressBar progress(
         {
            {w, "Cols"},
            {h, "Rows"}
         }
      );
      progress.setTitle("Ray Tracing Scene").render();

      for (auto it : progress) {
         auto col = it[0], row = it[1];

         auto i = col;
         auto j = h - 1 - row;

         Ray ray = camera->generate_ray(i, j);

         std::vector<Surfel> intersections;
         for (const auto& primitive : _scene.getPrimitives()) {
            Surfel sf(0.0f, Point3(), nullptr);
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
            auto color = closestSurfel.material->getColor(p);
            film.setPixel(color, j, i);
            continue;
         }

         float u_norm = static_cast<float>(i) / (w - 1);
         float v_norm = static_cast<float>(j) / (h - 1);

         auto color = background->sampleUV(u_norm, v_norm);
         film.setPixel(color, j, i);
      }

      film.save();
   }
   
   void Api::initEngine(const RunningOptions& options) {
      if (options.isFail()) {
         throw std::runtime_error("Failed to initialize engine due to invalid options.");
      }

      _options = options;
   }

   void Api::run() {
      // Parse data and save static Tags ParamSet
      _scene = ParserScene::parseScene(_options.getInputSceneFile().c_str());
      auto& params = _scene.getParams();

      if (params.find("film") == params.end())
         throw std::runtime_error("Scene data must contain 'film' parameters.");
      
      if (_options.hasOutput()) {
         std::string outputPath = _options.getOutput();
         params["film"].add<std::string>("filename", outputPath);
      }

      // Find world_end tag and call render() when found
      auto worldEndIt = params.find("world_end");
      if (worldEndIt != params.end()) {
         render();
      } else {
         std::cerr << "[Api] Warning: <world_end> tag not found. Rendering skipped.\n";
      }
   }

   void Api::cleanUp() {
      _options = RunningOptions(); // Reset options to default
      _scene = Scene(); // Reset scene to default
   }
}