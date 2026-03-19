#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Film.hpp"

namespace raytracer {
   enum CameraType {
      Perspective,
      Orthographic
   };

   class Camera {
      public:
         Film film;
         CameraType type;

         Camera(const Film& film, CameraType type):
            film(film), type(type) {}
         ~Camera() = default;

         Image capture() const;
   };
}

#endif // !CAMERA_HPP