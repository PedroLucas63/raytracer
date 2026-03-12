#include "Vector3.hpp"
#include <stdexcept>

namespace raytracer {
   /** Getters and Setters method */
   double Vector3::getX() const {
      return _x;
   }
   double Vector3::getY() const {
      return _y;
   }
   double Vector3::getZ() const {
      return _z;
   }
   void Vector3::setX(double x) {
      _x = x;
   }
   void Vector3::setY(double y) {
      _y = y;
   }
   void Vector3::setZ(double z) {
      _z = z;
   }

   /** Limit methods */
   Vector3 Vector3::clamp(double minValue, double maxValue) const {
      if (minValue > maxValue) 
         throw std::invalid_argument("minValue cannot be greater than maxValue");
      
      return clampMin(minValue).clampMax(maxValue);
   }
   Vector3 Vector3::clampMin(double value) const {
      double x = _x < value ? value : _x;
      double y = _y < value ? value : _y;
      double z = _z < value ? value : _z;

      return Vector3(x, y, z);
   }
   Vector3 Vector3::clampMax(double value) const {
      double x = _x > value ? value : _x;
      double y = _y > value ? value : _y;
      double z = _z > value ? value : _z;

      return Vector3(x, y, z);
   }
   Vector3 Vector3::abs() const {
      double x = _x < 0 ? -_x : _x;
      double y = _y < 0 ? -_y : _y;
      double z = _z < 0 ? -_z : _z;

      return Vector3(x, y, z);
   }

   /** General Methods */
   double Vector3::sum() const {
      return _x + _y + _z;
   }
   bool Vector3::isZero() const {
      return _x == 0 && _y == 0 && _z == 0;
   }
   double Vector3::min() const {
      if (_x < _y && _x < _z) return _x;
      else if (_y < _x && _y < _z) return _y;
      else return _z;
   }
   double Vector3::max() const {
      if (_x > _y && _x > _z) return _x;
      else if (_y > _x && _y > _z) return _y;
      else return _z;
   }
   Axis Vector3::minAxis() const {
      if (_x < _y && _x < _z) return X;
      else if (_y < _x && _y < _z) return Y;
      else return Z;
   }
   Axis Vector3::maxAxis() const {
      if (_x > _y && _x > _z) return X;
      else if (_y > _x && _y > _z) return Y;
      else return Z;
   }

   /** Access operator */
   double Vector3::operator[](Axis axis) const {
      switch (axis) {
         case X: return _x;
         case Y: return _y;
         case Z: return _z;
         default: throw std::invalid_argument("axis must be X, Y, or Z");
      }
   }

   /** Unary operators */
   Vector3 Vector3::operator-() const {
      return Vector3(-_x, -_y, -_z);
   }

   /** Binary operators */
   Vector3 Vector3::operator+(const Vector3& other) const {
      return Vector3(_x + other._x, _y + other._y, _z + other._z);
   }
   Vector3 Vector3::operator-(const Vector3& other) const {
      return Vector3(_x - other._x, _y - other._y, _z - other._z);
   }
   Vector3 Vector3::operator*(const Vector3& other) const {
      return Vector3(_x * other._x, _y * other._y, _z * other._z);
   }
   Vector3 Vector3::operator/(const Vector3& other) const {
      return Vector3(_x / other._x, _y / other._y, _z / other._z);
   }
   Vector3 Vector3::operator+(double other) const {
      return Vector3(_x + other, _y + other, _z + other);
   }
   Vector3 Vector3::operator-(double other) const {
      return Vector3(_x - other, _y - other, _z - other);
   }
   Vector3 Vector3::operator*(double other) const {
      return Vector3(_x * other, _y * other, _z * other);
   }
   Vector3 Vector3::operator/(double other) const {
      return Vector3(_x / other, _y / other, _z / other);
   }

   /** Assignment operators */
   Vector3& Vector3::operator+=(const Vector3& other) {
      _x += other._x;
      _y += other._y;
      _z += other._z;

      return *this;
   }
   Vector3& Vector3::operator-=(const Vector3& other) {
      _x -= other._x;
      _y -= other._y;
      _z -= other._z;

      return *this;
   }
   Vector3& Vector3::operator*=(const Vector3& other) {
      _x *= other._x;
      _y *= other._y;
      _z *= other._z;

      return *this;
   }
   Vector3& Vector3::operator/=(const Vector3& other) {
      _x /= other._x;
      _y /= other._y;
      _z /= other._z;

      return *this;
   }
   Vector3& Vector3::operator+=(double other) {
      _x += other;
      _y += other;
      _z += other;

      return *this;
   }
   Vector3& Vector3::operator-=(double other) {
      _x -= other;
      _y -= other;
      _z -= other;

      return *this;
   }
   Vector3& Vector3::operator*=(double other) {
      _x *= other;
      _y *= other;
      _z *= other;

      return *this;
   }
   Vector3& Vector3::operator/=(double other) {
      _x /= other;
      _y /= other;
      _z /= other;

      return *this;
   }

   /** Relational operators */
   bool Vector3::operator==(const Vector3& other) const {
      return _x == other._x && _y == other._y && _z == other._z;
   }
   bool Vector3::operator!=(const Vector3& other) const {
      return !(*this == other);
   }
}