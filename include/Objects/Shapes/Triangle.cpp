#include "Objects/Shapes/Triangle.hpp"
#include <unordered_map>

namespace raytracer {
   void Vertex::checkNormal() const {
      if (_normal.length() == 0) {
         throw std::invalid_argument("Cannot be assign vertex normal with length equal than 0.");
      }
   }

   void Vertex::checkTextureCoordinates() const {
      auto u = _textureCoordinates.getX();
      auto v = _textureCoordinates.getY();

      if (u < 0.0 || u > 1.0 || v < 0.0 || v > 1.0) {
         throw std::invalid_argument("You can assign texture coordinates with values beetween 0.0 and 1.0");
      }
   }

   Vertex::Vertex(
      const Point3& position, 
      const Point2& textureCoordinates,
      const RGBColor& color,
      const Vector3 normal
   ) : _position(position), 
      _textureCoordinates(textureCoordinates),
      _color(color), 
      _normal(normal) 
   {
      checkNormal();
      checkTextureCoordinates();
   }

   void Vertex::setPosition(const Point3& position) {
      _position = position;
   }
   void Vertex::setColor(const RGBColor& color) {
      _color = color;
   }
   void Vertex::setNormal(const Vector3& normal) {
      _normal = normal;
      checkNormal();
   }
   void Vertex::setTextureCoordinates(const Point2& textureCoordinates) {
      _textureCoordinates = textureCoordinates;
      checkTextureCoordinates();
   }

   Point3 Vertex::getPosition() const {
      return _position;
   }
   RGBColor Vertex::getColor() const {
      return _color;
   }
   Vector3 Vertex::getNormal() const {
      return _normal;
   }
   Point2 Vertex::getTextureCoordinates() const {
      return _textureCoordinates;
   }

   void Triangle::checkVertices() const {
      auto p1 = (*_vertesis[0]).getPosition();
      auto p2 = (*_vertesis[1]).getPosition();
      auto p3 = (*_vertesis[2]).getPosition();

      if (p1 == p2 || p1 == p3 || p2 == p3) {
         throw std::invalid_argument("Cannot create a triangle with same vertex");
      }
   }

   std::optional<float> Triangle::intersectRay(const Ray& ray) const {
      auto result = intersectRayWithUV(ray);
      if (result.has_value()) return result.value().first;
      return std::nullopt;
   }

   std::optional<std::pair<float, Point2>> Triangle::intersectRayWithUV(const Ray& ray) const {
      constexpr double EPSILON = std::numeric_limits<double>::epsilon();

      auto v0 = getVertex(0); 
      auto v1 = getVertex(1); 
      auto v2 = getVertex(2); 

      auto edge1 = v1.getPosition() - v0.getPosition();
      auto edge2 = v2.getPosition() - v0.getPosition();

      auto normal = edge1.cross(edge2);
      if (normal.dot(ray.direction) > 0 && _backfaceCull) return std::nullopt;

      auto rayCrossEdge2 = ray.direction.cross(edge2);
      auto determinant = edge1.dot(rayCrossEdge2);
      if (std::abs(determinant) < EPSILON) return std::nullopt;

      auto inverseDeterminant = 1.0 / determinant;
      auto rayToVertex = ray.origin - v0.getPosition();
      float u = inverseDeterminant * rayToVertex.dot(rayCrossEdge2);
      if (u < 0 || u > 1) return std::nullopt;

      auto vertexCrossEdge1 = rayToVertex.cross(edge1);
      float v = inverseDeterminant * ray.direction.dot(vertexCrossEdge1);
      if (v < 0 || u + v > 1) return std::nullopt;

      float t = inverseDeterminant * edge2.dot(vertexCrossEdge1);
      if (t < ray.t_min || t > ray.t_max) return std::nullopt;

      return std::make_pair(t, Point2(u, v));
   }

   Triangle::Triangle(const Triangle::Vertesis& vertesis, bool backfaceCull)
      : _vertesis(vertesis), _backfaceCull(backfaceCull), Shape()
   {
      checkVertices();
   }

   Triangle::Triangle(const std::shared_ptr<Vertex>& firstVextex, bool backfaceCull)
      : _backfaceCull(backfaceCull), Shape()
   {
      setVertices(firstVextex);
   }

   void Triangle::setVertices(const Triangle::Vertesis& vertesis) {
      _vertesis = vertesis;
      
   }

   void Triangle::setVertices(const std::shared_ptr<Vertex>& firstVextex) {
      Vertex* base = firstVextex.get();

      _vertesis = Vertesis{
         firstVextex,
         std::shared_ptr<Vertex>(firstVextex, base + 1),
         std::shared_ptr<Vertex>(firstVextex, base + 2)
      };

      checkVertices();
   }

   void Triangle::setBackfaceCull(bool backfaceCull) {
      _backfaceCull = backfaceCull;
   }

   Triangle::Vertesis Triangle::getVertices() const {
      return _vertesis;
   }

   Vertex Triangle::getVertex(uint index) const {
      if (index >= SIDES) {
         throw std::out_of_range("Index out of range for triangle vertices");
      }
      return *_vertesis[index];
   }

   Vertex Triangle::operator[](uint index) const {
      return getVertex(index);
   }

   bool Triangle::getBackfaceCull(bool backfaceCull) const {
      return _backfaceCull;
   }

   Vector3 Triangle::getFaceNormal() const {
      auto v0 = getVertex(0); 
      auto v1 = getVertex(1); 
      auto v2 = getVertex(2); 

      auto edge1 = v1.getPosition() - v0.getPosition();
      auto edge2 = v2.getPosition() - v0.getPosition();
      return edge1.cross(edge2).normalize();
   }

   Vector3 Triangle::getBarycentricNormal(const Point2& uv) const {
      auto u = uv.getX();
      auto v = uv.getY();

      auto n0 = getVertex(0).getNormal();
      auto n1 = getVertex(1).getNormal();
      auto n2 = getVertex(2).getNormal();

      auto normal = n0 * (1 - u - v) + n1 * u + n2 * v;
      return normal.normalize();
   }


   bool Triangle::intersect(const Ray& ray) const {
      return intersectRay(ray).has_value();
   }

   bool Triangle::intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const  {
      auto result = intersectRayWithUV(ray);
      if (!result.has_value()) return false;

      auto [t, uv] = result.value();

      *tHit = t;
      surfel->point = ray(*tHit);

      surfel->normal = getBarycentricNormal(uv);
      surfel->material = nullptr;

      return true;
   }

   Bounds3 Triangle::getBounds() const {
      Point3 min = (*_vertesis[0]).getPosition();
      Point3 max = min;

      for (size_t i = 1; i < SIDES; i++) {
         Point3 vertexPos = (*_vertesis[i]).getPosition();

         if (vertexPos.getX() < min.getX()) min.setX(vertexPos.getX());
         if (vertexPos.getY() < min.getY()) min.setY(vertexPos.getY());
         if (vertexPos.getZ() < min.getZ()) min.setZ(vertexPos.getZ());

         if (vertexPos.getX() > max.getX()) max.setX(vertexPos.getX());
         if (vertexPos.getY() > max.getY()) max.setY(vertexPos.getY());
         if (vertexPos.getZ() > max.getZ()) max.setZ(vertexPos.getZ());
      }

      return Bounds3(min, max);
   }

   TriangleMesh::TriangleMesh(const ParamSet& params) {
      uint ntriangles = std::numeric_limits<uint>::infinity();
      std::vector<uint> indices;

      if (params.has("indices")) {
         indices = params.retrieve<std::vector<uint>>("indices");

         if (indices.size() % 3 != 0)
            throw std::invalid_argument(""); // TODO: Add message
         
         ntriangles = indices.size() / 3; // TODO: Add message
      } else {
         throw std::invalid_argument("");
      }

      if (params.has("ntriangles")) {
         auto n = params.retrieve<uint>("ntriangles");
         if (n < ntriangles)
            ntriangles = n;
      }

      _reverseVertexOrder = params.retrieveOrDefault("reverse_vertex_order", false);
      _computeNormals = params.retrieveOrDefault("compute_normals", false);
      _backfaceCull = params.retrieveOrDefault("backface_cull", false);

      if (!params.has("vertices")) {
         throw std::invalid_argument("");// TODO: Add message
      }
      auto vertexPoints = params.retrieve<std::vector<Point3>>("vertices");

      if (!params.has("normals")) { // TODO: Normals is optional
         throw std::invalid_argument("");// TODO: Add message
      }
      auto vertexNormals = params.retrieve<std::vector<Point3>>("normals");

      if (!params.has("uv")) {
         throw std::invalid_argument("");// TODO: Add message
      }
      auto vertexTextureCoordinates = params.retrieve<std::vector<Point2>>("uv");

      // TODO: Permit to have less normals and texture coordinates than vertex points, but not more
      if (vertexPoints.size() != vertexNormals.size() || vertexPoints.size() != vertexTextureCoordinates.size()) {
         throw std::invalid_argument("");// TODO: Add message
      }

      for (size_t i = 0; i < vertexPoints.size(); i++) {
         _vertesis.push_back(std::make_shared<Vertex>(
            vertexPoints[i],
            vertexTextureCoordinates[i],
            PIXEL_BLACK,
            vertexNormals[i]
         ));
      }

      // TODO: Validate triangle indices
      for (size_t i = 0; i < ntriangles * 3; i++) {
         if (indices[i] >= _vertesis.size()) {
            throw std::invalid_argument(""); // TODO: Add message
         }
      }

      _triangleIndices = indices;
   }

   std::vector<std::shared_ptr<Triangle>> TriangleMesh::makeTriangules() {
      // [1] Create all triangles
      std::vector<std::shared_ptr<Triangle>> triangles;
      uint nTriangles = _triangleIndices.size() / 3;

      for (size_t i = 0; i < nTriangles; i++) {
         auto v0_i = _triangleIndices[i * 3];
         auto v1_i = _triangleIndices[i * 3 + 1];
         auto v2_i = _triangleIndices[i * 3 + 2];

         if (_reverseVertexOrder) {
            std::swap(v1_i, v2_i);
         }

         auto v0 = _vertesis[v0_i];
         auto v1 = _vertesis[v1_i];
         auto v2 = _vertesis[v2_i];

         auto triangle = std::make_shared<Triangle>(
            std::array<std::shared_ptr<Vertex>, 3>{v0, v1, v2},
            _backfaceCull
         );

         triangles.push_back(triangle);

         if (_computeNormals) {
            auto faceNormal = triangle->getFaceNormal();
            v0->setNormal(v0->getNormal() + faceNormal);
            v1->setNormal(v1->getNormal() + faceNormal);
            v2->setNormal(v2->getNormal() + faceNormal);
         }
      }

      if (_computeNormals) {
         for (auto& vertex : _vertesis) {
            auto normal = vertex->getNormal();
            vertex->setNormal(normal.normalize());
         }
      }
   }
}