#ifndef SURFEL_HPP
#define SURFEL_HPP

#include "Image/RGBColor.hpp"
#include "Objects/Material.hpp"
#include <memory>

namespace raytracer {
   class Surfel {
      public:
         Surfel(float t, Point3 point, std::shared_ptr<Material> material) 
            : t(t), point(point), material(material) {}
         ~Surfel() = default;

         float t; // Distance along the ray to the intersection point
         Point3 point;
         std::shared_ptr<Material> material;
   };
}

#endif // !SURFEL_HPP