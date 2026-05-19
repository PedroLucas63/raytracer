#ifndef BOX_HPP
#define BOX_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"
#include <tuple>


namespace raytracer{
    class Box : public Shape{

        private:
            Point3 _first;
            Point3 _second;

            std::pair<float, float> calculateIntersectPoints(const Ray& ray) const;

        public:

            Box (Point3 first, Point3 second) :
                _first(first), _second(second){}
            
            Box (const ParamSet& params);
            ~Box() = default;

            bool intersect(const Ray& ray) const override;
            bool intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const override;
            Bounds3 getBounds() const override;

    };
}




#endif // BOX_HPP