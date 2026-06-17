#include "Objects/Shapes/Cylinder.hpp"
#include "Math/Routines.hpp"
#include <cmath>
#include <limits>

namespace raytracer {
   Cylinder::Cylinder(const Point3& base, const Point3& top, float radius)
      : Shape(), _base(base), _top(top), _radius(radius) {}

   Cylinder::Cylinder(const ParamSet& params): Shape() {
      if (!params.has("base")) {
         throw std::invalid_argument("Cylinder requires a 'base' parameter");
      }
      if (!params.has("top")) {
         throw std::invalid_argument("Cylinder requires a 'top' parameter");
      }
      if (!params.has("radius")) {
         throw std::invalid_argument("Cylinder requires a 'radius' parameter");
      }

      _base = params.retrieve<Point3>("base");
      _top = params.retrieve<Point3>("top");
      _radius = params.retrieve<float>("radius");
   }

   bool Cylinder::intersect(
      const Ray& ray, float* tMin, float* tMax
   ) const {
      // PROTEÇÃO 1: Evitar lixo de memória caso solveQuadratic encontre apenas 1 raiz
      *tMin = INFINITY;
      *tMax = INFINITY;

      auto axis = _top - _base;
      float height = axis.length();
      auto a = axis.normalize();

      auto oc = ray.origin - _base;
      auto dPerp = ray.direction - a * ray.direction.dot(a);
      auto ocPerp = oc - a * oc.dot(a);

      auto A = dPerp.dot(dPerp);
      
      // PROTEÇÃO 2: Se A for próximo de 0, o raio é perfeitamente paralelo ao cilindro.
      // Ele nunca vai atingir as paredes laterais, evitando divisão por zero na Bhaskara.
      if (A < 1e-8f) {
         return false; 
      }

      auto B = 2 * dPerp.dot(ocPerp);
      auto C = ocPerp.dot(ocPerp) - _radius * _radius;

      if (!solveQuadratic(A, B, C, tMin, tMax)) {
         return false;
      }

      constexpr float EPSILON = 1e-4f;

      float t = *tMin;
      auto hitPoint = ray(t);
      auto hitHeight = (hitPoint - _base).dot(a);

      if (t < EPSILON || hitHeight < 0.0f || hitHeight > height) {
         t = *tMax;
         
         // PROTEÇÃO 3: Impede que use o segundo t se ele for infinito (1 raiz apenas) ou NaN
         if (t == INFINITY || std::isnan(t)) return false;

         hitPoint = ray(t);
         hitHeight = (hitPoint - _base).dot(a);

         if (t < EPSILON || hitHeight < 0.0f || hitHeight > height) {
            return false;
         }
      }

      *tMin = t;
      return true;
   }

   bool Cylinder::intersectCaps(const Ray& ray, float* t, bool* hitBottom) const {
      auto a = (_top - _base).normalize();
      auto denom = ray.direction.dot(a);

      constexpr float EPSILON = 1e-4f;

      if (std::abs(denom) < EPSILON) {
         return false; // Raio paralelo às tampas
      }

      float closest = INFINITY;
      bool hit = false;
      *hitBottom = false;   

      float t0 = (_base - ray.origin).dot(a) / denom;

      if (t0 > EPSILON) {
         auto hitPoint = ray(t0);
         auto d = hitPoint - _base;

         if (d.dot(d) <= _radius * _radius) {
            *hitBottom = true;
            closest = t0;
            hit = true;
         }
      }

      float t1 = (_top - ray.origin).dot(a) / denom;
      if (t1 > EPSILON) {
         auto hitPoint = ray(t1);
         auto d = hitPoint - _top;

         if (d.dot(d) <= _radius * _radius) {
            if (t1 < closest) {
               *hitBottom = false;
               closest = t1;
            }
            hit = true;
         }
      }

      if (hit) *t = closest;

      return hit;
   }

   bool Cylinder::intersect(const Ray& ray) const {
      // PROTEÇÃO 4: Inicialização de sanidade das variáveis locas
      float tMinSide = INFINITY, tMaxSide = INFINITY, tMinCap = INFINITY;
      bool hitBottom = false;

      bool hitSide = intersect(ray, &tMinSide, &tMaxSide);
      bool hitCap = intersectCaps(ray, &tMinCap, &hitBottom);

      return hitSide || hitCap;
   }

   bool Cylinder::intersectWithSurfel(
      const Ray& ray, float* tHit, Surfel* sf
   ) const {
      // PROTEÇÃO 4 (cont.): Inicialização de sanidade das variáveis locais
      float tMinSide = INFINITY, tMaxSide = INFINITY, tMinCap = INFINITY;
      bool hitBottom = false;

      bool hitSide = intersect(ray, &tMinSide, &tMaxSide);
      bool hitCap = intersectCaps(ray, &tMinCap, &hitBottom);

      if (!hitSide && !hitCap) return false;

      Point3 hitPointSide;
      Vector3 normalSide;
      if (hitSide) {
         auto hitPoint = ray(tMinSide);
         auto axis = _top - _base;
         auto a = axis.normalize();
         auto hitHeight = (hitPoint - _base).dot(a);

         auto axisPoint = _base + a * hitHeight;

         hitPointSide = hitPoint;
         normalSide = (hitPoint - axisPoint).normalize();
      }

      Point3 hitPointCap;
      Vector3 normalCap;
      if (hitCap) {
         auto hitPoint = ray(tMinCap);
         auto a = (_top - _base).normalize();

         hitPointCap = hitPoint;
         normalCap = hitBottom ? -a : a;
      }

      if (hitSide && hitCap) {
         bool capWins = (tMinCap < tMinSide + 1e-3f);

         auto hit = capWins ? tMinCap : tMinSide;
         auto point = capWins ? hitPointCap : hitPointSide;
         auto normal = capWins ? normalCap : normalSide;

         if (tHit) *tHit = hit;
         if (sf) {
            sf->point = point;
            sf->normal = normal;
         }
      } else if (hitSide) {
         if (tHit) *tHit = tMinSide;
         if (sf) {
            sf->point = hitPointSide;
            sf->normal = normalSide;
         }
      } else {
         if (tHit) *tHit = tMinCap;
         if (sf) {
            sf->point = hitPointCap;
            sf->normal = normalCap;
         }
      }

      if (sf) {
         sf->viewDir = -ray.direction.normalize();
      }

      return hitSide || hitCap;
   }

   Bounds3 Cylinder::getBounds() const {
      Bounds3 bounds = Bounds3::fromPoints(_base, _top);

      Vector3 axis = (_top - _base).normalize();
      Vector3 expand(
         _radius * std::sqrt(1.0f - axis.getX() * axis.getX()),
         _radius * std::sqrt(1.0f - axis.getY() * axis.getY()),
         _radius * std::sqrt(1.0f - axis.getZ() * axis.getZ())
      );

      return Bounds3(
         bounds.min() - expand,
         bounds.max() + expand
      );
   }
}