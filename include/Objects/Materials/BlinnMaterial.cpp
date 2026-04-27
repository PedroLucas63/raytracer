#include "Objects/Materials/BlinnMaterial.hpp"

namespace raytracer {
   BlinnMaterial::BlinnMaterial(const ParamSet& params) : Material(params) {
      if (params.has("diffuse")) {
         setDiffuse(params.retrieve<Vector3>("diffuse"));
      } else {
         throw std::invalid_argument("BlinnMaterial requires a 'diffuse' parameter of type Vector3");
      }

      if (params.has("specular")) {
         setSpecular(params.retrieve<Vector3>("specular"));
      } else {
         throw std::invalid_argument("BlinnMaterial requires a 'specular' parameter of type Vector3");
      }

      if (params.has("ambient")) {
         setAmbient(params.retrieve<Vector3>("ambient"));
      } else {
         throw std::invalid_argument("BlinnMaterial requires an 'ambient' parameter of type Vector3");
      }

      if (params.has("glossiness")) {
         _glossiness = params.retrieve<float>("glossiness");
      } else {
         throw std::invalid_argument("BlinnMaterial requires a 'glossiness' parameter of type float");
      }
   }

   Vector3 BlinnMaterial::getDiffuse() const {
      return _diffuse;
   }

   Vector3 BlinnMaterial::getSpecular() const {
      return _specular;
   }

   Vector3 BlinnMaterial::getAmbient() const {
      return _ambient;
   }

   float BlinnMaterial::getGlossiness() const {
      return _glossiness;
   }

   void BlinnMaterial::setDiffuse(const Vector3& diffuse) {
      _diffuse = diffuse.clamp(0.0, 1.0);
   }

   void BlinnMaterial::setSpecular(const Vector3& specular) {
      _specular = specular.clamp(0.0, 1.0);
   }

   void BlinnMaterial::setAmbient(const Vector3& ambient) {
      _ambient = ambient.clamp(0.0, 1.0);
   }

   void BlinnMaterial::setGlossiness(float glossiness) {
      _glossiness = glossiness;
   }

   const RGBColor& BlinnMaterial::getColor(const Point3& point) const {
      // TODO: Implement Blinn-Phong shading model to calculate the color based on the material properties and the point's position
      return RGBColor();
   }
}