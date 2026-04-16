#include "Bounds3.hpp"

namespace raytracer {
   const Point3& Bounds3::min() const {
      return _min;
   }

   const Point3& Bounds3::max() const {
      return _max;
   }
}