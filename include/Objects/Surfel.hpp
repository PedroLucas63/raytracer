#ifndef SURFEL_HPP
#define SURFEL_HPP

#include "Image/RGBColor.hpp"
#include "Objects/Materials/Material.hpp"
#include "Math/Point3.hpp"
#include <memory>

namespace raytracer {
   class Surfel {
      public:
         Surfel(float t = 0, Point3 point = Point3(0, 0, 0), std::shared_ptr<Material> material = nullptr) 
            : t(t), point(point), material(material) {}
         ~Surfel() = default;

         float t; // Distance along the ray to the intersection point
         Point3 point;
         std::shared_ptr<Material> material;
   };
}

#endif // !SURFEL_HPP