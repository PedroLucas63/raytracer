#include "Core/GraphicsState.hpp"
#include "Api/Api.hpp"

namespace raytracer {
   GraphicsState::GraphicsState() {
      _currMaterial.push(nullptr);
      _materialStack.push_back(DictOfMat());
   }

   void GraphicsState::addMaterial(const std::shared_ptr<Material>& material) {
      _currMaterial.pop();
      _currMaterial.push(material);
      _materialStack.back()[material->getName()] = material;
   }

   std::shared_ptr<Material> GraphicsState::getActivatedMaterial() const {
      if (_currMaterial.empty()) {
         throw std::runtime_error("No activated material set");
      }

      return _currMaterial.top();
   }

   void GraphicsState::addNamedMaterial(const std::shared_ptr<Material>& material) {
      if (material->isAnonymous()) {
         throw std::invalid_argument("make_named_material requires a named material (set 'name' attribute).");
      }

      addMaterial(material);
   }

   std::shared_ptr<Material> GraphicsState::getMaterialAt(const std::string& name) const {
      auto dict = _materialStack.rbegin();
      auto it = dict->find(name);

      if (it != dict->end()) {
         if (it->second->isAnonymous()) {
            throw std::invalid_argument("Cannot get an anonymous material");
         }

         return it->second;
      }

      if (!_materialsCloned) {
         throw std::runtime_error("Material not found: " + name);
      }

      while (++dict != _materialStack.rend()) {
         it = dict->find(name);

         if (it != dict->end()) {
            if (it->second->isAnonymous()) {
               throw std::invalid_argument("Cannot get an anonymous material");
            }

            return it->second;
         }
      }

      throw std::runtime_error("Material not found: " + name);
   }

   void GraphicsState::activateNamedMaterial(const std::string& name) {
      auto material = getMaterialAt(name);
      _currMaterial.pop();
      _currMaterial.push(material);
   }

   void GraphicsState::addAggregate(const std::shared_ptr<AggregatePrimitive>& aggregate) {
      _scene.addAggregate(aggregate);
   }

   void GraphicsState::addPrimitives(const std::shared_ptr<AggregatePrimitive>& primitives) {
      std::shared_ptr<Transform> transform;
      bool flipNormals;
      Api::getCurrentTransform(&transform, &flipNormals);
      _scene.addPrimitives(primitives, *transform);
   }
 
   bool GraphicsState::hasAggregate() const {
      return _scene.hasAggregate();
   }

   void GraphicsState::defineNewObject(std::string name) {
      _scene.defineNewObject(name);
   }

   void GraphicsState::saveNewObject() {
      _scene.saveNewObject();
   }

   void GraphicsState::instanciateObject(
      const std::string& name
   ) {
      std::shared_ptr<Transform> transform;
      bool flipNormals;
      Api::getCurrentTransform(&transform, &flipNormals);
      _scene.instanciateObject(name, *transform);
   }

   void GraphicsState::addLight(const std::shared_ptr<Light>& light) {
      _scene.addLight(light);
   }

   void GraphicsState::setParam(const std::string& key, const ParamSet& value) {
      _scene.setParam(key, value);
   }

   void GraphicsState::push() {
      _currMaterial.push(_materialsCloned ? _currMaterial.top() : nullptr);
      _materialStack.push_back(DictOfMat());
   }

   void GraphicsState::pop() {
      if (_materialStack.size() <= 1) {
         throw std::runtime_error("Cannot pop the last graphics state");
      }

      _currMaterial.pop();
      _materialStack.pop_back();
   }

   bool GraphicsState::empty() const {
      return _materialStack.size() <= 1;
   }

   void GraphicsState::include(const GraphicsState& other) {
      static const std::shared_ptr<Transform> transformIdentity = std::make_shared<Transform>();

      _scene.include(other._scene, *transformIdentity);

      if (other._currMaterial.empty()) {
         throw std::runtime_error("Cannot include a graphics state with no activated material");
      }

      _currMaterial.pop();
      _currMaterial.push(other._currMaterial.top());

      for (const auto& dict : other._materialStack) {
         DictOfMat newDict;
         for (const auto& pair : dict) {
            newDict[pair.first] = pair.second;
         }
         _materialStack.push_back(newDict);
      }
   }
}