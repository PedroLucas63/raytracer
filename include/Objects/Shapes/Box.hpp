#ifndef BOX_HPP
#define BOX_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"
#include <tuple>


namespace raytracer{
    class Box : public Shape{
        private:
            static const Point3 _first;
            static const Point3 _second;

            Vector3 computeNormal(const Ray& ray, float tHit) const;
        public:

            Box() = default;
            ~Box() = default;

            bool intersect(const Ray& ray, const Transform& objToWorld) const override;
            bool intersectWithSurfel(const Ray& ray, const Transform& objToWorld, float* tHit, Surfel* sf) const override;
            Bounds3 getBounds(const Transform& objToWorld) const override;
    };
}




#endif // BOX_HPP