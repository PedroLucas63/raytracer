#include "Scene/Scene.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include <iostream>

namespace raytracer {   

   Scene& Scene::operator=(const Scene& other) {
      if (this != &other) {
         _aggregate = other._aggregate;
         _materialMap = other._materialMap;
         _lastMaterial = other._lastMaterial;
         _background = other._background ? other._background : nullptr;
         _params = other._params;
      }
      return *this;
   }
   
   void Scene::addPrimitive(const std::shared_ptr<Primitive>& primitive) {
      _aggregate->add(primitive);
   }

   bool Scene::intersect(const Ray& r, Surfel* isect) const {
      return _aggregate->intersectWithSurfel(r, isect);
   }
 
   bool Scene::intersect_p(const Ray& r) const {
      return _aggregate->intersect(r);
   }

   void Scene::addMaterial(const std::shared_ptr<Material>& material) {
      _lastMaterial = material;
      _materialMap[material->getName()] = material;
   }

   void Scene::addNamedMaterial(const std::shared_ptr<Material>& material) {
      // Stores in the library but does NOT change _lastMaterial
      if (material->isAnonymous()) {
         throw std::invalid_argument("make_named_material requires a named material (set 'name' attribute).");
      }
      _materialMap[material->getName()] = material;
   }

   void Scene::activateNamedMaterial(const std::string& name) {
      auto it = _materialMap.find(name);
      if (it != _materialMap.end()) {
         _lastMaterial = it->second;
      } else {
         throw std::runtime_error("Material not found: " + name);
      }
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

   // void Scene::include(const Scene& other) {
   //    _params.insert(other._params.begin(), other._params.end());

   //    if (other._params.find("background") != other._params.end()) {
   //       buildBackground();
   //    }

   //    _lastMaterial = other._lastMaterial;
   //    _materialMap.insert(other._materialMap.begin(), other._materialMap.end());
   //    _primitives.insert(
   //       _primitives.end(), other._primitives.begin(), other._primitives.end()
   //    );
   // }

   void Scene::include(const Scene& other) {
      _params.insert(other._params.begin(), other._params.end());
 
      if (other._params.find("background") != other._params.end()) {
         buildBackground();
      }
 
      _lastMaterial = other._lastMaterial;
      _materialMap.insert(other._materialMap.begin(), other._materialMap.end());
 
      const auto& otherAggregate = other._aggregate;
      if (otherAggregate) {
         _aggregate->merge(otherAggregate);
      }
   }
}