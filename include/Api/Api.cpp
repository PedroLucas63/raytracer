#include "Api.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include "Utils/ProgressBar.hpp"

namespace raytracer {
   // Define the static member variable
   RunningOptions Api::_options;
   ParamSets Api::_sceneData;

   void Api::render() {
      auto film = std::make_shared<Film>(_sceneData["film"]); 
      // auto camera = CameraFactory::build(film, _sceneData); 
      auto background = BackgroundFactory::build(_sceneData);

      int w = film->getWidth();
      int h = film->getHeight();

      ProgressBar progress(
         {
            {h, "Rows"}, 
            {w, "Cols"}
         }
      );
      progress.setTitle("Ray Tracing Scene");

      for (auto it : progress) {
         auto row = it[0], col = it[1];

         //todo: generate ray for current pixel using camera

      }

      film->save();
   }
   
   void Api::initEngine(const RunningOptions& options) {
      if (options.isFail()) {
         throw std::runtime_error("Failed to initialize engine due to invalid options.");
      }

      _options = options;
   }

   void Api::run() {
      // Parse data and save static Tags ParamSet
      _sceneData = ParserScene::parseScene(_options.getInputSceneFile().c_str());

      if (_sceneData.find("film") == _sceneData.end())
         throw std::runtime_error("Scene data must contain 'film' parameters.");
      
      if (_options.hasOutput()) {
         std::string outputPath = _options.getOutput();
         _sceneData["film"].add<std::string>("filename", outputPath);
      }

      // Find world_end tag and call render() when found
      auto worldEndIt = _sceneData.find("world_end");
      if (worldEndIt != _sceneData.end()) {
         render();
      } else {
         std::cerr << "[Api] Warning: <world_end> tag not found. Rendering skipped.\n";
      }
   }

   void Api::cleanUp() {
      _options = RunningOptions(); // Reset options to default
      _sceneData.clear(); // Clear scene data
   }
}