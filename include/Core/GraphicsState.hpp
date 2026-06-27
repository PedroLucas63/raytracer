#ifndef GRAPHICS_STATE_HPP
#define GRAPHICS_STATE_HPP

#include "Scene/Scene.hpp"
#include <list>
#include <unordered_map>
#include <stack>

namespace raytracer {
   class GraphicsState {
      private:
         using DictOfMat = std::unordered_map<std::string, std::shared_ptr<Material>>;

         Scene _scene;
         std::stack<std::shared_ptr<Material>> _currMaterial;
         std::list<DictOfMat> _materialStack;
         bool _flipNormals = false;
         bool _materialsCloned = false;

      public:
         GraphicsState();

         void setFlipNormals(bool flip) { _flipNormals = flip; }
         bool getFlipNormals() const { return _flipNormals; }
         void setMaterialsCloned(bool cloned) { _materialsCloned = cloned; }
         bool getMaterialsCloned() const { return _materialsCloned; }

         const Scene& getScene() const { return _scene; }
         bool isDefiningObject() const { return _scene.isDefiningObject(); }

         void addMaterial(const std::shared_ptr<Material>& material);
         std::shared_ptr<Material> getActivatedMaterial() const;

         void addNamedMaterial(const std::shared_ptr<Material>& material);
         std::shared_ptr<Material> getMaterialAt(const std::string& name) const;
         void activateNamedMaterial(const std::string& name);

         void addAggregate(const std::shared_ptr<AggregatePrimitive>& aggregate);
         void addPrimitives(const std::shared_ptr<AggregatePrimitive>& primitives);
         bool hasAggregate() const;

         void defineNewObject(std::string name);
         void saveNewObject();
         void instanciateObject(const std::string& name);

         void addLight(const std::shared_ptr<Light>& light);

         void setParam(const std::string& key, const ParamSet& value);

         void push();
         void pop();
         bool empty() const;

         void include(const GraphicsState& other);
   };
}

#endif // GRAPHICS_STATE_HPP