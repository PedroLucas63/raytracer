#ifndef POINT2_HPP
#define POINT2_HPP

#include "Axis.hpp"
#include <iterator>

namespace raytracer {
   class Point2 {
      private:
         double _x, _y;
      
      public:
         /** Constructors */
         constexpr Point2(): _x(0.0), _y(0.0) {}
         constexpr Point2(double x, double y):
            _x(x), _y(y) {}
         
         template <std::input_iterator Iter>
         requires std::same_as<std::iter_value_t<Iter>, double>
         constexpr Point2(Iter begin, Iter end) {
            auto it = begin;

            if (it == end) throw std::invalid_argument("Point2 needs at least 2 values");
            _x = *it++;

            if (it == end) throw std::invalid_argument("Point2 needs at least 2 values");
            _y = *it++;
         }
         
         
         /** Destructor */ 
         ~Point2() = default;

         /** Getters and Setters method */
         double getX() const;
         double getY() const;
         void setX(double x);
         void setY(double y);

         /** Access operator */
         double operator[](Axis axis) const;
   };
}

#endif // !POINT2_HPP