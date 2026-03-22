#include "Api.hpp"

namespace raytracer {
   // Define the static member variable
   RunningOptions Api::_options;

   void Api::render() {
      // Initialize camera, film, and background static objects
      // Render the scene
      // Save the rendered image
   }  
   
   void Api::initEngine(const RunningOptions& options) {
      if (options.isFail()) {
         throw std::runtime_error("Failed to initialize engine due to invalid options.");
      }

      _options = options;
   }

   void Api::run() {
      // Parse data and save static Tags ParamSet
      // Find world_end tag and call render() when found
   }

   void Api::cleanUp() {
      // Clean up any resources if necessary
      _options = RunningOptions(); // Reset options to default
   }

   void Api::film(const raytracer::ParamSet& ps) {
      // Implementation for film function
   }

}