#include "Scene/Scene.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include <iostream>

namespace raytracer {   

   Scene& Scene::operator=(const Scene& other) {
      if (this != &other) {
         _primitives = other._primitives;
         _materialMap = other._materialMap;
         _lastMaterial = other._lastMaterial;
         _background = other._background ? other._background : nullptr;
         _params = other._params;
      }
      return *this;
   }
   
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

   void Scene::setBackground(const Background& background) {
      _background = background.clone();
   }

   void Scene::buildBackground() {
      if (!_background)
         _background = BackgroundFactory::create(_params);
   }

   const std::shared_ptr<Background> Scene::getBackground() const {
      if (!_background) {
         throw std::runtime_error("Background not set");
      }
      return _background;
   }

   bool Scene::hasBackground() const {
      return _background != nullptr;
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

      if (key == "background") {
         buildBackground();
      }
   }

   const ParamSets& Scene::getParams() const { 
      return _params; 
   }

   void Scene::include(const Scene& other) {
      _params.insert(other._params.begin(), other._params.end());

      if (other._params.find("background") != other._params.end()) {
         buildBackground();
      }

      _lastMaterial = other._lastMaterial;
      _materialMap.insert(other._materialMap.begin(), other._materialMap.end());
      _primitives.insert(
         _primitives.end(), other._primitives.begin(), other._primitives.end()
      );
   }
}