#include "Point2.hpp"
#include <stdexcept>

namespace raytracer {
   double Point2::getX() const {
      return _x;
   }

   double Point2::getY() const {
      return _y;
   }

   void Point2::setX(double x) {
      _x = x;
   }

   void Point2::setY(double y) {
      _y = y;
   }

   double Point2::operator[](Axis axis) const {
      switch (axis) {
         case Axis::X:
            return _x;
         case Axis::Y:
            return _y;
         default:
            throw std::out_of_range("Invalid axis");
      }
   }
}