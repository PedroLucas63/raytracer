#include "Utils/TensorUtils.hpp"

namespace raytracer {
   Tensor<double> identity() {
      return Tensor<double>::eye(4);
   }

   Point3 applyPoint(
      const Tensor<double>& transform, const Point3& point
   ) {
      if (transform.shape() != std::vector<size_t>{4, 4}) {
         throw std::invalid_argument("Transform must be a 4x4 matrix.");
      }

      double x = point.getX();
      double y = point.getY();
      double z = point.getZ();

      double xp = transform({0, 0}) * x + transform({0, 1}) * y + transform({0, 2}) * z + transform({0, 3});
      double yp = transform({1, 0}) * x + transform({1, 1}) * y + transform({1, 2}) * z + transform({1, 3});
      double zp = transform({2, 0}) * x + transform({2, 1}) * y + transform({2, 2}) * z + transform({2, 3});
      double wp = transform({3, 0}) * x + transform({3, 1}) * y + transform({3, 2}) * z + transform({3, 3});

      if (wp == 1.0) {
         return Point3(xp, yp, zp);
      } else {
         return Point3(xp / wp, yp / wp, zp / wp);
      }
   }

   Vector3 applyVector(
      const Tensor<double>& transform, const Vector3& vector
   ) {
      if (transform.shape() != std::vector<size_t>{4, 4}) {
         throw std::invalid_argument("Transform must be a 4x4 matrix.");
      }

      double x = vector.getX();
      double y = vector.getY();
      double z = vector.getZ();

      return Vector3(
         transform({0, 0}) * x + transform({0, 1}) * y + transform({0, 2}) * z,
         transform({1, 0}) * x + transform({1, 1}) * y + transform({1, 2}) * z,
         transform({2, 0}) * x + transform({2, 1}) * y + transform({2, 2}) * z
      );
   }

   Vector3 applyNormal(
      const Tensor<double>& transform, const Vector3& normal
   ) {
      if (transform.shape() != std::vector<size_t>{4, 4}) {
         throw std::invalid_argument("Transform must be a 4x4 matrix.");
      }

      Tensor<double> invTransform = transform.inverse();
      Vector3 transformedNormal = applyVector(invTransform.transpose(), normal);
      return transformedNormal.normalize();
   }

   Ray applyRay(
      const Tensor<double>& transform, const Ray& ray
   ) {
      Point3 newOrigin = applyPoint(transform, ray.origin);
      Vector3 newDirection = applyVector(transform, ray.direction);
      return Ray(newOrigin, newDirection, ray.t_min, ray.t_max);
   }

   Bounds3 applyBounds(
      const Tensor<double>& transform, const Bounds3& bounds
   ) {
      Point3 oldMin = bounds.min();
      Point3 oldMax = bounds.max();

      Point3 first = applyPoint(transform, oldMin);
      Point3 newMin = first;
      Point3 newMax = first;


      for (int i = 1; i < 8; ++i) {
         double x = (i & 1) ? oldMax.getX() : oldMin.getX();
         double y = (i & 2) ? oldMax.getY() : oldMin.getY();
         double z = (i & 4) ? oldMax.getZ() : oldMin.getZ();
         
         Point3 corner(x, y, z);
         Point3 transformedCorner = applyPoint(transform, corner);

         newMin.setX(std::min(newMin.getX(), transformedCorner.getX()));
         newMin.setY(std::min(newMin.getY(), transformedCorner.getY()));
         newMin.setZ(std::min(newMin.getZ(), transformedCorner.getZ()));

         newMax.setX(std::max(newMax.getX(), transformedCorner.getX()));
         newMax.setY(std::max(newMax.getY(), transformedCorner.getY()));
         newMax.setZ(std::max(newMax.getZ(), transformedCorner.getZ()));
      }
      
      return Bounds3(newMin, newMax);
   }

   Tensor<double> translation(const Vector3& translation) {
      double tx = translation.getX();
      double ty = translation.getY();
      double tz = translation.getZ();

      return Tensor<double>::from_data({
         {1, 0, 0, tx},
         {0, 1, 0, ty},
         {0, 0, 1, tz},
         {0, 0, 0, 1}
      });
   }

   Tensor<double> scale(const Vector3& scale) {
      double sx = scale.getX();
      double sy = scale.getY();
      double sz = scale.getZ();

      return Tensor<double>::from_data({
         {sx, 0, 0, 0},
         {0, sy, 0, 0},
         {0, 0, sz, 0},
         {0, 0, 0, 1}
      });
   }

   Tensor<double> rotation(const Vector3& axis, double angleDeg) {
      double angleRad = angleDeg * M_PI / 180.0;
      double c = std::cos(angleRad);
      double s = std::sin(angleRad);
      double t = 1 - c;

      const double len = axis.length();
      if (len < 1e-6)
         throw std::invalid_argument("rotation: axis vector has zero length");

      double ax = axis.getX() / len;
      double ay = axis.getY() / len;
      double az = axis.getZ() / len;

      return Tensor<double>::from_data({
         {t*ax*ax + c,    t*ax*ay - s*az, t*ax*az + s*ay, 0},
         {t*ax*ay + s*az, t*ay*ay + c,    t*ay*az - s*ax, 0},
         {t*ax*az - s*ay, t*ay*az + s*ax, t*az*az + c,    0},
         {0, 0, 0, 1}
      });
   }

   Tensor<double> compose(const Tensor<double>& parent, const Tensor<double>& child) {
      return parent.dot(child);
   }
}