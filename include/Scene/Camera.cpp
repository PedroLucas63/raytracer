#include "Camera.hpp"

namespace raytracer {
   Image Camera::capture() const {
      return film.getImage();
   }
}