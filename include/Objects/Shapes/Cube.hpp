#ifndef CUBE_HPP
#define CUVE_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"
#include <tuple>


namespace raytracer{
    class Cube : public Shape{

        private:
            Point3 _first;
            Point3 _second;

            std::pair<float, float> calculateIntersectPoints(const Ray& ray) const;

        public:

            Cube (Point3 first, Point3 second) :
                _first(first), _second(second){}
            
            Cube (const ParamSet& params);
            ~Cube() = default;

            bool intersect(const Ray& ray) const override;
            bool intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const override;

    };
}




#endif // CUBE_HPP