#include "Scene.hpp"

namespace raytracer {   
   
   void Scene::addPrimitive(const std::shared_ptr<Primitive>& primitive) {
      _primitives.push_back(primitive);
   }

   void Scene::addMaterial(const std::shared_ptr<Material>& material) {
      _materialMap[material->getName()] = material;
   }


   const std::vector<std::shared_ptr<Primitive>>& Scene::getPrimitives() const {
      return _primitives;
   }

   Material* Scene::getMaterialAt(const std::string& name) const{
      auto it = _materialMap.find(name);
      if (it != _materialMap.end()) {
         return it->second.get();
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


}