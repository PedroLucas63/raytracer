#ifndef POINT2_HPP
#define POINT2_HPP

#include "Axis.hpp"

namespace raytracer {
   class Point2 {
      private:
         double _x, _y;
      
      public:
         /** Constructors */
         constexpr Point2(): _x(0.0), _y(0.0) {}
         constexpr Point2(double x, double y):
            _x(x), _y(y) {}
         
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