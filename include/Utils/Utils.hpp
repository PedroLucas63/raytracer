#ifndef UTILS_HPP
#define UTILS_HPP

#include "Image/RGBColor.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   RGBColor multiplyColorByIntensity(const RGBColor& color, const Vector3& intensity);
}

#endif // !UTILS_HPP