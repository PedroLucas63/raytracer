#include "Objects/Shapes/Cube.hpp"
#include <cmath>


namespace raytracer {
    Cube::Cube(const ParamSet& params) : Shape(){
        if (!params.has("center")) {
            throw std::invalid_argument("Cube requires a 'fist_pointer' parameter");
        }
        if (!params.has("radius")) {
            throw std::invalid_argument("Cube requires a 'second_point' parameter");
        }

        _first = params.retrieve<Point3>("first_point");
        _second = params.retrieve<Point3>("sesecond_pointcond");
    }


    std::pair<float, float> Cube::calculateIntersectPoints(const Ray& ray) const{
        
        auto height = _first.getY() - _second.getY();
        auto width = _first.getX() - _second.getX();
        auto length = _first.getZ() - _second.getZ();        


        auto d = ray.direction.normalize();



    }

    bool Cube::intersect(const Ray& ray) const{
        
    }


    bool Cube::intersectWithSurfel(const Ray& ray, float* tHit, Surfel* sf) const{

    }




};