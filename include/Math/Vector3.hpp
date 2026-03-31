#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <stddef.h>
#include <limits>
#include "Axis.hpp"
#include <iterator>
#include <ostream>

namespace raytracer {
   class Vector3 {
      private:
         double _x, _y, _z;
      public:
         /** Constructors */
         constexpr Vector3(): _x(0.0), _y(0.0), _z(0.0) {}
         constexpr Vector3(double x, double y, double z):
            _x(x), _y(y), _z(z) {}
         template <std::input_iterator Iter>
         requires std::same_as<std::iter_value_t<Iter>, double>
         constexpr Vector3(Iter begin, Iter end) {
            auto it = begin;

            if (it == end) throw std::invalid_argument("Vector3 needs at least 3 values");
            _x = *it++;

            if (it == end) throw std::invalid_argument("Vector3 needs at least 3 values");
            _y = *it++;

            if (it == end) throw std::invalid_argument("Vector3 needs at least 3 values");
            _z = *it++;
         }
         
         
         /** Destructor */
         ~Vector3() = default;

         /** Getters and Setters method */
         double getX() const;
         double getY() const;
         double getZ() const;
         void setX(double x);
         void setY(double y);
         void setZ(double z);

         /** Limit methods */
         Vector3 clamp(double minValue, double maxValue) const;
         Vector3 clampMin(double value) const;
         Vector3 clampMax(double value) const;
         Vector3 abs() const;

         /** General Methods */
         double sum() const;
         bool isZero() const;
         double min() const;
         double max() const;
         Axis minAxis() const;
         Axis maxAxis() const;

         /** Math Methods */
         double  length() const;
         double  lengthSquared() const;
         Vector3 normalize() const;
         double  dot(const Vector3& other) const;
         Vector3 cross(const Vector3& other) const;
         
         /** Access operator */
         double operator[](Axis axis) const;

         /** Unary operators */
         Vector3 operator-() const;

         /** Binary operators */
         Vector3 operator+(const Vector3& other) const;
         Vector3 operator-(const Vector3& other) const;
         Vector3 operator*(const Vector3& other) const;
         Vector3 operator/(const Vector3& other) const;
         Vector3 operator+(double other) const;
         Vector3 operator-(double other) const;
         Vector3 operator*(double other) const;
         Vector3 operator/(double other) const;

         /** Assignment operators */
         Vector3& operator+=(const Vector3& other);
         Vector3& operator-=(const Vector3& other);
         Vector3& operator*=(const Vector3& other);
         Vector3& operator/=(const Vector3& other);
         Vector3& operator+=(double other);
         Vector3& operator-=(double other);
         Vector3& operator*=(double other);
         Vector3& operator/=(double other);

         /** Relational operators */
         bool operator==(const Vector3& other) const;
         bool operator!=(const Vector3& other) const;
   };

   /** Constants */
   constexpr const Vector3 VECTOR3_ZERO(0.0, 0.0, 0.0);
   constexpr const Vector3 VECTOR3_ONE(1.0, 1.0, 1.0);
   constexpr const Vector3 VECTOR3_UNIT_X(1.0, 0.0, 0.0);
   constexpr const Vector3 VECTOR3_UNIT_Y(0.0, 1.0, 0.0);
   constexpr const Vector3 VECTOR3_UNIT_Z(0.0, 0.0, 1.0);
}

#endif // !VECTOR3_HPP