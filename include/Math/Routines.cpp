#include "Math/Routines.hpp"
#include <cmath>

namespace raytracer {
   bool solveQuadratic(
      float a, float b, float c, float* t0, float* t1
   ) {
      double da = (double) a;
      double db = (double) b;
      double dc = (double) c;

      double discriminant = db * db - 4 * da * dc;
      if (discriminant < 0) return false;

      double rootDiscriminant = std::sqrt(discriminant);

      double q = db < 0 ? -0.5 * (b - rootDiscriminant) : -0.5 * (b + rootDiscriminant);
      *t0 = q / a;
      *t1 = c / q;

      if (*t0 > *t1) std::swap(*t0, *t1);
      return true;
   }

   Point3 min(const Point3& a, const Point3& b) {
      return Point3(
         std::min(a.getX(), b.getX()),
         std::min(a.getY(), b.getY()),
         std::min(a.getZ(), b.getZ())
      );
   }

   Point3 max(const Point3& a, const Point3& b) {
      return Point3(
         std::max(a.getX(), b.getX()),
         std::max(a.getY(), b.getY()),
         std::max(a.getZ(), b.getZ())
      );
   }
}