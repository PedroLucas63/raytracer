#include "Api.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include "Utils/ProgressBar.hpp"
#include <iostream>
#include "Objects/Shapes/Sphere.hpp"
#include "Objects/Surfel.hpp"
#include "Integrator/IntegratorFactory.hpp"

namespace raytracer {
   // Define the static member variable
   RunningOptions Api::_options;
   Scene Api::_scene;

   void Api::generate() {
      auto integrator = IntegratorFactory::create(_scene.getParams());

      integrator->render(_scene);

      if (_options.hasOutput())
         integrator->saveImage(_options.getOutput(), _options.noOverwrite());
      else
         integrator->saveImage(_options.noOverwrite());
   }
   
   void Api::initEngine(const RunningOptions& options) {
      if (options.isFail()) {
         throw std::runtime_error("Failed to initialize engine due to invalid options.");
      }

      _options = options;
   }

   void Api::run() {
      // Parse data and save static Tags ParamSet
      ParserScene::parseScene(_options.getInputSceneFile().c_str(), _scene);
      auto& params = _scene.getParams();

      if (params.find("film") == params.end())
         throw std::runtime_error("Scene data must contain 'film' parameters.");

      // Find world_end tag and call render() when found
      auto worldEndIt = params.find("world_end");
      if (worldEndIt != params.end()) {
         generate();
      } else {
         std::cerr << "[Api] Warning: <world_end> tag not found. Rendering skipped.\n";
      }
   }

   void Api::cleanUp() {
      _options = RunningOptions(); // Reset options to default
      _scene = Scene(); // Reset scene to default
   }
}