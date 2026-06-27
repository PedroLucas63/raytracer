#ifndef BOUNDS3_HPP
#define BOUNDS3_HPP

#include "Math/Point3.hpp"
#include "Math/Ray.hpp"
#include "Math/Axis.hpp"
#include <limits>

namespace raytracer {
   class Bounds3 {
      private:
         Point3 _min;
         Point3 _max;
         bool _isInfinity = false;

      public:
         Bounds3() {}
         Bounds3(const Point3& min, const Point3& max): 
            _min(min), _max(max) {}
         static Bounds3 fromPoints(const Point3& p1, const Point3& p2);
         static Bounds3 infinite();
         ~Bounds3() = default;

         const Point3& min() const;
         const Point3& max() const;

         Bounds3 merge(const Bounds3& bounds) const;
         Vector3 diagonal() const;
         Axis maxExtent() const;

         bool intersect(
            const Ray& ray, 
            float &hit1, 
            float &hit2
         ) const;
   };
}

#endif // !BOUNDS3_HPP