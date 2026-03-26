#include "Point3.hpp"
#include <stdexcept>

namespace raytracer {
   /** Getters and Setters method */
   double Point3::getX() const {
      return _x;
   }
   double Point3::getY() const {
      return _y;
   }
   double Point3::getZ() const {
      return _z;
   }
   void Point3::setX(double x) {
      _x = x;
   }
   void Point3::setY(double y) {
      _y = y;
   }
   void Point3::setZ(double z) {
      _z = z;
   }

   /** Access operator */
   double Point3::operator[](Axis axis) const {
      switch (axis) {
         case Axis::X: return _x;
         case Axis::Y: return _y;
         case Axis::Z: return _z;
         default: throw std::invalid_argument("Invalid axis");
      }
   }
}