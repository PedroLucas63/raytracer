#ifndef POINT3_HPP
#define POINT3_HPP

#include "Point2.hpp"
#include "Vector3.hpp"
#include "Axis.hpp"

namespace raytracer {
   class Point3 {
      private:
         double _x;
         double _y;
         double _z;
         
      public:
         /** Constructors */
         constexpr Point3(): _x(0), _y(0), _z(0) {};
         constexpr Point3(double x, double y, double z): _x(x), _y(y), _z(z) {};
         Point3(const Point2& point, double z = 0): 
            _x(point.getX()), _y(point.getY()), _z(z) {};
         Point3(const Point3& other): 
            _x(other._x), _y(other._y), _z(other._z) {};
         
         /** Destructor */
         ~Point3() = default;

         /** Getters and Setters methods */
         double getX() const;
         double getY() const;
         double getZ() const;
         void setX(double x);
         void setY(double y);
         void setZ(double z);

         /** Access operator */
         double operator[](Axis axis) const;

         // Point + vector → deslocated point
         Point3 operator+(const Vector3& v) const {
            return Point3(_x + v.getX(), _y + v.getY(), _z + v.getZ());
         }
         // Point - vector → deslocated point
         Point3 operator-(const Vector3& v) const {
            return Point3(_x - v.getX(), _y - v.getY(), _z - v.getZ());
         }
         // Point - point → vector between the two points
         Vector3 operator-(const Point3& other) const {
            return Vector3(_x - other._x, _y - other._y, _z - other._z);
         }
 
         bool operator==(const Point3& other) const {
            return _x == other._x && _y == other._y && _z == other._z;
         }
         bool operator!=(const Point3& other) const {
            return !(*this == other);
         }
   };
}

#endif // !POINT3_HPP