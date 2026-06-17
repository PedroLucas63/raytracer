#ifndef ROUTINES_HPP
#define ROUTINES_HPP

#include "Math/Point3.hpp"

namespace raytracer {
   bool solveQuadratic(float a, float b, float c, float* t0, float* t1);
   Point3 min(const Point3& a, const Point3& b);
   Point3 max(const Point3& a, const Point3& b);
}

#endif // !ROUTINES_HPP