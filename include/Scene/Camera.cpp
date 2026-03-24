#include "Camera.hpp"

namespace raytracer {
   Camera::Camera(const ParamSets& params): film(1, 1, 3) { // Default to a 1x1 image until properly initialized
      auto it = params.find("camera");
      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'camera' parameters.");

      const ParamSet& cameraParams = it->second;
      const std::string typeParam = cameraParams.retrieveOrDefault<std::string>("type", "orthographic");

      if (typeParam == "orthographic")
         type = CameraType::Orthographic;
      else
         throw std::invalid_argument("Unsupported camera type: " + type);

      film = Film(params);
   }
   
   Image Camera::capture() const {
      return film.getImage();
   }
}