#include "Scene/Scene.hpp"

namespace raytracer {   
   
   void Scene::addPrimitive(const std::shared_ptr<Primitive>& primitive) {
      _primitives.push_back(primitive);
   }

   void Scene::addMaterial(const std::shared_ptr<Material>& material) {
      _lastMaterial = material;
      _materialMap[material->getName()] = material;
   }


   const std::vector<std::shared_ptr<Primitive>>& Scene::getPrimitives() const {
      return _primitives;
   }

   std::shared_ptr<Material> Scene::getMaterialAt(const std::string& name) const{
      auto it = _materialMap.find(name);
      if (it != _materialMap.end()) {
         return it->second;
      }
      throw std::runtime_error("Material not found: " + name);
   }

   ParamSet Scene::getParam(const std::string& key) const {
      auto it = _params.find(key);
      if (it != _params.end()) {
         return it->second;
      }
      throw std::runtime_error("Parameter not found: " + key);
   }

   void Scene::setParam(const std::string& key, const ParamSet& value) {
      _params[key] = value;
   }

   ParamSets& Scene::getParams() { 
      return _params; 
   }

   void Scene::include(const Scene& other) {
      _params.insert(other._params.begin(), other._params.end());
      _lastMaterial = other._lastMaterial;
      _materialMap.insert(other._materialMap.begin(), other._materialMap.end());
      _primitives.insert(
         _primitives.end(), other._primitives.begin(), other._primitives.end()
      );
   }
}