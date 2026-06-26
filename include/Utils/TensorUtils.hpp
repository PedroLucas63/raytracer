#ifndef TENSOR_UTILS_HPP
#define TENSOR_UTILS_HPP

#include "Math/Tensor/Tensor.hpp"
#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include "Math/Ray.hpp"
#include "Math/Bounds3.hpp"

namespace raytracer {
   Tensor<double> identity();
   Point3 applyPoint(
      const Tensor<double>& transform, const Point3& point
   );
   Vector3 applyVector(
      const Tensor<double>& transform, const Vector3& vector
   );
   Vector3 applyNormal(
      const Tensor<double>& transform, const Vector3& normal
   );
   Ray applyRay(
      const Tensor<double>& transform, const Ray& ray
   );
   Bounds3 applyBounds(
      const Tensor<double>& transform, const Bounds3& bounds
   );
   Tensor<double> translation(const Vector3& translation);
   Tensor<double> translation(const Vector3& translation);
   Tensor<double> rotation(const Vector3& axis, double angleDeg);
   Tensor<double> compose(const Tensor<double>& parent, const Tensor<double>& child);
}

#endif // TENSOR_UTILS_HPP