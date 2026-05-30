#include "Objects/Shapes/Triangle.hpp"
#include <unordered_map>
#include <tiny_obj_loader.h>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION


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
      auto cross = edge1.cross(edge2);
      if (cross.length() == 0.0f)
         return VECTOR3_UNIT_Y; // degenerate triangle fallback
      return cross.normalize();
   }

   Vector3 Triangle::getBarycentricNormal(const Point2& uv) const {
      auto u = uv.getX();
      auto v = uv.getY();

      auto n0 = getVertex(0).getNormal();
      auto n1 = getVertex(1).getNormal();
      auto n2 = getVertex(2).getNormal();

      auto normal = n0 * (1 - u - v) + n1 * u + n2 * v;
      if (normal.length() > 0.0f)
         return normal.normalize();
      // fallback: use geometric face normal
      return getFaceNormal();
   }


   bool Triangle::intersect(const Ray& ray) const {
      return intersectRay(ray).has_value();
   }

   bool Triangle::intersectWithSurfel(const Ray& ray, float *tHit, Surfel* surfel) const  {
      auto result = intersectRayWithUV(ray);
      if (!result.has_value()) return false;

      auto [t, uv] = result.value();

      if (tHit)
         *tHit = t;

      if (!surfel)
         return true;

      surfel->point   = ray(t);
      surfel->normal  = getBarycentricNormal(uv);
      surfel->viewDir = -ray.direction.normalize();
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

      _reverseVertexOrder = params.retrieveOrDefault("reverse_vertex_order", false);
      _computeNormals     = params.retrieveOrDefault("compute_normals", false);
      _backfaceCull       = params.retrieveOrDefault("backface_cull", false);
      _nTriangles         = 0;
      
      if (params.has("filename")) {
         loadByObjectFile(params.retrieve<std::string>("filename"));
      }else{
         loadFromParamSet(params);
      }
   }

   void TriangleMesh::loadByObjectFile(std::string filename) {

      tinyobj::attrib_t attrib;
      std::vector<tinyobj::shape_t> shapes;
      std::vector<tinyobj::material_t> materials;
      std::string warn, err;

      bool ret = tinyobj::LoadObj(&attrib,
         &shapes,
         &materials, 
         &warn, 
         &err, 
         filename.c_str(), 
         nullptr, 
         true //always triangulate faces
      ); 

      if (!warn.empty()) {
         std::cerr << "WARN: " << warn << std::endl;
      }if (!err.empty()) {
         throw std::runtime_error("Failed to load OBJ file: " + err);
      }if (shapes.empty()) {
         throw std::runtime_error("No shapes found in OBJ file: " + filename);
      }if (!ret) {
         throw std::runtime_error("Failed to load OBJ file: " + filename);
      }

      const size_t nVerts = attrib.vertices.size() / 3;
      const size_t nNormals = attrib.normals.size() / 3;
      const size_t nUVs = attrib.texcoords.size() / 2;
   
      _vertesis.reserve(nVerts);
      _normals.reserve(nNormals);
      _uvcoords.reserve(nUVs);

      for (size_t i = 0; i < nVerts; i++){
         _vertesis.emplace_back(
            attrib.vertices[3 * i],
            attrib.vertices[3 * i + 1],
            attrib.vertices[3 * i + 2]
         );
      }
      
      for (size_t i = 0; i < nNormals; i++){
         _normals.emplace_back(
            attrib.normals[3 * i],
            attrib.normals[3 * i + 1],
            attrib.normals[3 * i + 2]
         );
      }

      for (size_t i = 0; i < nUVs; i++){
         _uvcoords.emplace_back( 
            std::clamp(attrib.texcoords[2 * i], 0.0f, 1.0f),
            std::clamp(attrib.texcoords[2 * i + 1], 0.0f, 1.0f)
         );
      }
      
      _nTriangles = 0;
      for (const auto& shape : shapes) {
         size_t indexOffset = 0;
         for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++){
            const size_t fv = shape.mesh.num_face_vertices[f];

            for (size_t v = 0; v < fv; v++) {
               int realVi = _reverseVertexOrder ? (fv - 1 - v) : v; // backwards vertex order if needed

               tinyobj::index_t idx = shape.mesh.indices[indexOffset + realVi];
               _vertexIndices.push_back(idx.vertex_index);
               _normalIndices.push_back(idx.normal_index);
               _uvIndices.push_back(idx.texcoord_index);
            }
            indexOffset += fv;
            _nTriangles++;
         }
      }
   }


   void TriangleMesh::loadFromParamSet(const ParamSet& params) {

      std::vector<uint> indices;

      if (params.has("indices")) {
         indices = params.retrieve<std::vector<uint>>("indices");

         if (indices.size() % 3 != 0)
            throw std::invalid_argument("Invalid number of indices for triangles");
         
         _nTriangles = indices.size() / 3;
      } else {
         throw std::invalid_argument("No indices provided for triangle mesh");
      }

      if (params.has("ntriangles")) {
         auto n = params.retrieve<uint>("ntriangles");
         if (n < _nTriangles)
            _nTriangles = n;
      }

      if (!params.has("vertices")) {
         throw std::invalid_argument("No vertices provided for triangle mesh");
      }
      _vertesis = params.retrieve<std::vector<Point3>>("vertices");

      bool hasNormals = params.has("normals");

      if (hasNormals) {
         _normals = params.retrieve<std::vector<Vector3>>("normals");
      }

      if (!params.has("uv")) {
         throw std::invalid_argument("No texture coordinates provided for triangle mesh");
      }
      _uvcoords = params.retrieve<std::vector<Point2>>("uv");

      if (_vertesis.size() != _uvcoords.size()) {
         throw std::invalid_argument("Inconsistent number of vertices and texture coordinates");
      }

      if (_normals.size() > _vertesis.size()) {
         throw std::invalid_argument("Inconsistent number of vertices and normals");
      }

      for (size_t i = 0; i < _nTriangles * 3; i++) {
         int idx = indices[i];

         if (idx >= _vertesis.size()) {
            throw std::invalid_argument("Invalid vertex index in triangle mesh");
         }

         _vertexIndices.push_back(idx);

         if(!_normals.empty() && idx < _normals.size()){
            _normalIndices.push_back(idx);
         }  else{
            _normalIndices.push_back(-1);
         }

         if(!_uvcoords.empty() && idx < _uvcoords.size()){
            _uvIndices.push_back(idx);
         } else{
            _uvIndices.push_back(-1);
         }
      }

   }

   Point2 TriangleMesh::getUV(uint triIdx, uint vertInTri) const {
      int ui = _uvIndices[triIdx * 3 + vertInTri];
      if (ui >= 0 && static_cast<size_t>(ui) < _uvcoords.size())
         return _uvcoords[ui];
      return Point2(0.0f, 0.0f);
   }

   Vector3 TriangleMesh::getNormal(uint triIdx, uint vertInTri, const Point3& p0, const Point3& p1, const Point3& p2) const {
      int ni = _normalIndices[triIdx * 3 + vertInTri];
      if (ni >= 0 && static_cast<size_t>(ni) < _normals.size())
         return _normals[ni];

      // Fallback: compute geometric face normal
      auto e1 = p1 - p0;
      auto e2 = p2 - p0;
      auto cross = e1.cross(e2);
      if (cross.length() == 0.0f)
         return VECTOR3_UNIT_Y; // degenerate triangle
      return cross.normalize();
   }

   std::vector<std::shared_ptr<Triangle>> TriangleMesh::makeTriangules() {

      std::vector<std::shared_ptr<Triangle>> triangles;
      triangles.reserve(_nTriangles);

      std::vector<Vector3> accumulatedNormals(_vertesis.size(), VECTOR3_ZERO);

      for (size_t i = 0; i < _nTriangles; i++) {
         auto v0_i = _vertexIndices[i * 3];
         auto v1_i = _vertexIndices[i * 3 + 1];
         auto v2_i = _vertexIndices[i * 3 + 2];

         if (_reverseVertexOrder) {
            std::swap(v1_i, v2_i);
         }

         auto v0 = _vertesis[v0_i];
         auto v1 = _vertesis[v1_i];
         auto v2 = _vertesis[v2_i];

         auto uv0 = getUV(i, 0);
         auto uv1 = getUV(i, 1);
         auto uv2 = getUV(i, 2);

         auto n0 = getNormal(i, 0, v0, v1, v2);
         auto n1 = getNormal(i, 1, v0, v1, v2);
         auto n2 = getNormal(i, 2, v0, v1, v2); 

         auto vertex0 = std::make_shared<Vertex>(v0, uv0, PIXEL_BLACK, n0);
         auto vertex1 = std::make_shared<Vertex>(v1, uv1, PIXEL_BLACK, n1);
         auto vertex2 = std::make_shared<Vertex>(v2, uv2, PIXEL_BLACK, n2);

         auto triangle = std::make_shared<Triangle>(
            std::array<std::shared_ptr<Vertex>, 3>{vertex0, vertex1, vertex2},
            _backfaceCull
         );

         triangles.push_back(triangle);

         if (_computeNormals) {
            auto faceNormal = triangle->getFaceNormal();
            accumulatedNormals[v0_i] += faceNormal;
            accumulatedNormals[v1_i] += faceNormal;
            accumulatedNormals[v2_i] += faceNormal;
         }
      }

      if (_computeNormals) {
         for (int i = 0; i < _nTriangles; i++) {
            auto vertesis = triangles[i]->getVertices();
            for (size_t k = 0; k < 3; k++) {
               int vi = _vertexIndices[i * 3 + k];
               auto& accum = accumulatedNormals[vi];
               if (accum.length() > 0.0f)
                  vertesis[k]->setNormal(accum.normalize());
            }
         }
      }

      return triangles;
   }
}