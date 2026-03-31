#ifndef RAY_HPP
#define RAY_HPP

#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include <limits>

namespace raytracer {
    class Ray {
        public:
            Point3  origin;       
            Vector3 direction;      
            mutable double t_min;
            mutable double t_max;
    
            Ray()
                : origin{}, direction{}, t_min{0.0}, t_max{std::numeric_limits<double>::infinity()}
            {}
    
            Ray(const Point3& origin, const Vector3& direction,
                double start = 0.0,
                double end   = std::numeric_limits<double>::infinity())
                : origin{origin}, direction{direction}, t_min{start}, t_max{end}
            {}
    
            // P(t) = o + t*d
            Point3 operator()(double t) const {
                return origin + direction * t;
            }
    };
}

#endif // RAY_HPP