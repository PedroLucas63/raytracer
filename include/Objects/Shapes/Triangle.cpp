#include "Objects/Shapes/Triangle.hpp"

namespace raytracer {
   void Vertex::checkNormal() const {
      if (_normal.length() == 0) {
         throw std::invalid_argument("Cannot be assign vertex normal with length equal than 0.");
      }
   }
   Vertex::Vertex(const Point3& position, const RGBColor& color)
      : _position(position), _color(color), _normal(VECTOR3_UNIT_Y) {}

   Vertex::Vertex(const Point3& position, const RGBColor& color, const Vector3 normal)
      : _position(position), _color(color), _normal(normal)
   {
      checkNormal();
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

   Point3 Vertex::getPosition() const {
      return _position;
   }
   RGBColor Vertex::getColor() const {
      return _color;
   }
   Vector3 Vertex::getNormal() const {
      return _normal;
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

      return t;
   }


   Triangle::Triangle(const Triangle::Vertesis& vertesis, bool backfaceCull)
      : _vertesis(vertesis), _backfaceCull(backfaceCull) 
   {
      checkVertices();
   }

   Triangle::Triangle(const std::shared_ptr<Vertex>& firstVextex, bool backfaceCull)
      : _backfaceCull(backfaceCull) 
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

   bool Triangle::intersect(const Ray& ray) const {
      return intersectRay(ray).has_value();
   }

   bool Triangle::intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const  {
      auto t = intersectRay(ray);
      if (!t.has_value()) return false;

      *tHit = t.value();
      surfel->point = ray(*tHit);

      auto v0 = getVertex(0); 
      auto v1 = getVertex(1); 
      auto v2 = getVertex(2); 

      auto edge1 = v1.getPosition() - v0.getPosition();
      auto edge2 = v2.getPosition() - v0.getPosition();
      surfel->normal = edge1.cross(edge2).normalize();

      surfel->material = nullptr; // TODO: assign material to triangle and set it here

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
}