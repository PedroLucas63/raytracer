#include "Api.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include "Utils/ProgressBar.hpp"
#include <iostream>
#include "Objects/Shapes/Sphere.hpp"
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

      std::unordered_map<std::string, std::function<void(Scene&)>> handlers;


      handlers["render_again"] = [&](Scene& s) {
         generate();
      };

      handlers["world_end"] = [&](Scene& s) {
         generate();
      };

      // The callback is invoked after every element is processed.
      // Api decides what each element means.
      ParserScene::parseScene(
         _options.getInputSceneFile().c_str(),
         _scene,
         [&](Scene& sceneRef, const std::string& element, const ParamSet&) {
            if (handlers.count(element)) {
               handlers[element](sceneRef);
            }
         }
      );
   }

   void Api::cleanUp() {
      _options = RunningOptions(); // Reset options to default
      _scene = Scene(); // Reset scene to default
   }
}