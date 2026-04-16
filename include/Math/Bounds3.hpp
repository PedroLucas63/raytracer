#ifndef BOUNDS3_HPP
#define BOUNDS3_HPP

#include "Math/Point3.hpp"
#include "Math/Ray.hpp"

namespace raytracer {
   class Bounds3 {
      private:
         Point3 _min;
         Point3 _max;

      public:
         Bounds3(const Point3& min, const Point3& max): 
            _min(min), _max(max) {}
         ~Bounds3() = default;

         const Point3& min() const;
         const Point3& max() const;
   };
}

#endif // !BOUNDS3_HPP