#include "Api.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Background.hpp"
#include "Utils/ProgressBar.hpp"

namespace raytracer {
   // Define the static member variable
   RunningOptions Api::_options;
   ParamSets Api::_sceneData;

   void Api::render() {
      // Initialize camera, film, and background static objects
      Camera camera(_sceneData);
      Background background(_sceneData);

      // Render the scene
      ProgressBar progress(
         {
            {camera.film.getHeight(), "Rows"},
            {camera.film.getWidth(), "Cols"}
         }
      );
      progress.setTitle("Rendering Scene");

      for (auto it : progress) {
         auto row = it[0], col = it[1];

         // Sample the background color for the current pixel
         auto color = background.samplePixel(
            {static_cast<double>(col), static_cast<double>(row)}, 
            camera.film.getWidth(), 
            camera.film.getHeight()
         );
         
         // Set the pixel color in the film
         camera.film.setPixel(color, row, col);
      }

      // Save the rendered image
      camera.film.save();
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