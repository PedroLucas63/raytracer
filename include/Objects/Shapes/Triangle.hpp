#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include "Image/RGBColor.hpp"
#include <memory>
#include <array>
#include <optional>

namespace raytracer {
   class Vertex {
      private:
         Point3 _position;
         RGBColor _color;
         Vector3 _normal;

         void checkNormal() const;

      public:
         Vertex(const Point3& position, const RGBColor& color);
         Vertex(const Point3& position, const RGBColor& color, const Vector3 normal);
         ~Vertex() = default;

         void setPosition(const Point3& position);
         void setColor(const RGBColor& color);
         void setNormal(const Vector3& normal);

         Point3 getPosition() const;
         RGBColor getColor() const;
         Vector3 getNormal() const;
   };

   class Edge {
   };

   class Triangle : public Shape {
      private:
         const static uint SIDES = 3;
         using Vertesis = std::array<std::shared_ptr<Vertex>, SIDES>;

         Vertesis _vertesis;
         bool _backfaceCull;

         void checkVertices() const;
         std::optional<float> intersectRay(const Ray& ray) const;
      
      public:
         Triangle(const Vertesis& vertesis, bool backfaceCull);
         Triangle(const std::shared_ptr<Vertex>& firstVextex, bool backfaceCull);

         ~Triangle() = default;

         void setVertices(const Vertesis& vertesis);
         void setVertices(const std::shared_ptr<Vertex>& firstVextex);
         void setBackfaceCull(bool backfaceCull);

         Vertesis getVertices() const;
         Vertex getVertex(uint index) const;
         Vertex operator[](uint index) const;
         bool getBackfaceCull(bool backfaceCull) const;

         bool intersect(const Ray& ray) const;
         bool intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const;
         Bounds3 getBounds() const;
   };
}

#endif // TRIANGLE_HPP