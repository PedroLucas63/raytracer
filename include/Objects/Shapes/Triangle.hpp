#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Objects/Shapes/Shape.hpp"
#include "Math/Point3.hpp"
#include "Math/Point2.hpp"
#include "Math/Vector3.hpp"
#include "Image/RGBColor.hpp"
#include <memory>
#include <array>
#include <optional>
#include <vector>

namespace raytracer {
   class Vertex {
      private:
         Point3 _position;
         Point2 _textureCoordinates;
         RGBColor _color;
         Vector3 _normal;

         void checkNormal() const;
         void checkTextureCoordinates() const;

      public:
         Vertex(
            const Point3& position, 
            const Point2& textureCoordinates,
            const RGBColor& color,
            const Vector3 normal = VECTOR3_UNIT_Y
         );
         ~Vertex() = default;

         void setPosition(const Point3& position);
         void setColor(const RGBColor& color);
         void setNormal(const Vector3& normal);
         void setTextureCoordinates(const Point2& textureCoordinates);

         Point3 getPosition() const;
         RGBColor getColor() const;
         Vector3 getNormal() const;
         Point2 getTextureCoordinates() const;
   };

   class Triangle : public Shape {
      private:
         const static uint SIDES = 3;
         using Vertesis = std::array<std::shared_ptr<Vertex>, SIDES>;

         Vertesis _vertesis;
         bool _backfaceCull;

         void checkVertices() const;
         std::optional<float> intersectRay(const Ray& ray) const;
         std::optional<std::pair<float, Point2>> intersectRayWithUV(const Ray& ray) const;
      
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
         Vector3 getFaceNormal() const;
         Vector3 getBarycentricNormal(const Point2& uv) const;

         bool intersect(const Ray& ray) const;
         bool intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const;
         Bounds3 getBounds() const;

   };

   class TriangleMesh {
      private:
         std::vector<Point3> _vertesis;
         std::vector<Vector3> _normals;
         std::vector<Point2> _uvcoords;

         std::vector<int> _vertexIndices;
         std::vector<int> _normalIndices;
         std::vector<int> _uvIndices;
 
         int _nTriangles;
         bool _reverseVertexOrder;
         bool _computeNormals;
         bool _backfaceCull;

         void loadByObjectFile(std::string filename);
         void loadFromParamSet(const ParamSet& params);
         Point2 getUV(uint triIdx, uint vertInTri) const;
         Vector3 getNormal(uint triIdx, uint vertInTri, const Point3& p0, const Point3& p1, const Point3& p2) const;

      public:
         TriangleMesh(const ParamSet& params);
         ~TriangleMesh() = default;
         std::vector<std::shared_ptr<Triangle>> makeTriangules();
   };
}

#endif // TRIANGLE_HPP