#include "Scene/Scene.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include "Objects/Aggregate/BVHAccel.hpp"
#include "Objects/Aggregate/LinearBVHAccel.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include <iostream>

namespace raytracer {   

   Scene& Scene::operator=(const Scene& other) {
      if (this != &other) {
         _instances = other._instances;
         _background = other._background ? other._background : nullptr;
         _params = other._params;
      }
      return *this;
   }
   
   void Scene::addAggregate(const std::shared_ptr<AggregatePrimitive>& aggregate) {
      _instances = aggregate;
   }

   void Scene::addPrimitives(const std::shared_ptr<AggregatePrimitive>& primitives, const Transform& transform) {
      if (!_instances) {
         _instances = std::make_shared<PrimitiveList>();
      }

      if (_currentObject) {
         _currentObject->merge(primitives, transform);
      } else {
         _instances->merge(primitives, transform);
      }
   }

   void Scene::instanciateObject(const std::string& name, std::shared_ptr<Transform> transform) {
      if (!_instances) {
         throw std::runtime_error("Cannot instantiate object: no aggregate primitive set in the scene.");
      }

      auto it = _objects.find(name);

      if (it != _objects.end()) {
         auto obj = it->second;
         auto primitive = obj.primitive;
         auto t = std::make_shared<Transform>((*transform) * (*obj.transform));
         
         if (_currentObject) {
           if (auto aggregatePrimitive = std::dynamic_pointer_cast<AggregatePrimitive>(primitive)) {
               _currentObject->merge(aggregatePrimitive, *t);
            } else {
               _currentObject->add({primitive, t});
            }
            return;
         }

         if (auto aggregatePrimitive = std::dynamic_pointer_cast<AggregatePrimitive>(primitive)) {
            _instances->merge(aggregatePrimitive, *t);
         } else {
            _instances->add({primitive, t});
         }
      } else {
         throw std::runtime_error("Object not found: " + name);
      }
   }

   bool Scene::intersectWithSurfel(const Ray& r, const Transform& transform, Surfel* isect) const {
      if (!_instances) {
         return false;
      }

      return _instances->intersectWithSurfel(r, transform, isect);
   }

   bool Scene::intersectWithSurfel(const Ray& r, Surfel* isect) const {
      return intersectWithSurfel(r, Transform(), isect);
   }
 
   bool Scene::intersect(const Ray& r, const Transform& transform) const {
      if (!_instances) {
         return false;
      }

      return _instances->intersect(r, transform);
   }

   bool Scene::intersect(const Ray& r) const {
      return intersect(r, Transform());
   }

   void Scene::addLight(const std::shared_ptr<Light>& light) {
      if (auto ambientLight = std::dynamic_pointer_cast<AmbientLight>(light)) {
         if (_ambientLight) {
            throw std::runtime_error("Scene already has an ambient light. Multiple ambient lights are not supported.");
         }
         
         _ambientLight = ambientLight;
      } else {
         _lights.push_back(light);
      }
   }

   const std::vector<std::shared_ptr<Light>>& Scene::getLights() const {
      return _lights;
   }

   std::shared_ptr<AmbientLight> Scene::getAmbientLight() const {
      return _ambientLight;
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

   void Scene::include(const Scene& other, const Transform& transform) {
      _params.insert(other._params.begin(), other._params.end());
 
      if (other._params.find("background") != other._params.end()) {
         buildBackground();
      }
 
      const auto& otherInstances = other._instances;

      if (_currentObject) {
         if (otherInstances)
            _currentObject->merge(otherInstances, transform);
      } else if (_instances && otherInstances) {
         _instances->merge(otherInstances, transform);
      } else if (otherInstances) {
         _instances = otherInstances;
      }
   }

   void Scene::prepareAggregate(const Transform& transform) {
      if (auto lbvh = std::dynamic_pointer_cast<LinearBVHAccel>(_instances)) {
         lbvh->buildLBVH(transform);
      } else if (auto bvh = std::dynamic_pointer_cast<BVHAccel>(_instances)) {
         bvh->buildBVH(transform);
      }
   }

   void Scene::prepareAggregate() {
      prepareAggregate(Transform());
   }

   bool Scene::hasAggregate() const {
      return _instances != nullptr;
   }

   void Scene::defineNewObject(std::string name) {
      if (_currentObject) {
         throw std::runtime_error("A new object is already being defined. Call saveNewObject() before defining another.");
      }
      _currentObjectName = name;

      if (_objects.find(name) != _objects.end()) {
         throw std::runtime_error("An object with the name '" + name + "' already exists. Choose a different name.");
      }

      _currentObject = std::make_shared<PrimitiveList>();
   }
   void Scene::saveNewObject() {
      if (!_currentObject) {
         throw std::runtime_error("No object is currently being defined. Call defineNewObject() before saving.");
      }

      Object newObject;
      newObject.name = _currentObjectName;
      newObject.primitive = _currentObject;
      newObject.transform = std::make_shared<Transform>();

      _objects[_currentObjectName] = newObject;

      _currentObject = nullptr;
   }
}