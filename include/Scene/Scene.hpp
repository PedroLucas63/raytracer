#ifndef SCENE_HPP
#define SCENE_HPP

#include "Scene/Background/Background.hpp"
#include "Objects/Primitive.hpp"
#include "Objects/Materials/Material.hpp"
#include "Objects/Materials/MaterialFactory.hpp"
#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Light/Light.hpp"
#include "Objects/Light/AmbientLight.hpp"
#include "Objects/Aggregate/BVHAccel.hpp"

#include <vector>
#include <memory>
#include <unordered_map>

namespace raytracer {
   using ParamSets = std::unordered_map<std::string, ParamSet>;

   struct Object {
      std::string name;
      std::shared_ptr<Primitive> primitive;
   };

   class Scene {
      private:
         std::shared_ptr<AggregatePrimitive> _instances;
         std::unordered_map<std::string, Object> _objects;
         std::vector<std::shared_ptr<Light>> _lights;
         std::shared_ptr<AmbientLight> _ambientLight = nullptr;

         std::shared_ptr<Background> _background;

         std::string _currentObjectName;
         std::shared_ptr<BVHAccel> _currentObject = nullptr;

         ParamSets _params;
         
      public:
         Scene() = default;
         ~Scene() = default;
         Scene& operator=(const Scene& other);

         bool intersectWithSurfel(const Ray& r, const Transform& transform, Surfel* surfel) const;
         bool intersectWithSurfel(const Ray& r, Surfel* surfel) const;
         bool intersect(const Ray& r, const Transform& transform) const;
         bool intersect(const Ray& r) const;

         bool hasAggregate() const;
         bool isDefiningObject() const { return _currentObject != nullptr; }
         void addAggregate(const std::shared_ptr<AggregatePrimitive>& aggregate);
         void addPrimitives(const std::shared_ptr<AggregatePrimitive>& primitives, const Transform& transform);
         
         void instanciateObject(const std::string& name, const Transform& transform);

         void addLight(const std::shared_ptr<Light>& light);
         const std::vector<std::shared_ptr<Light>>& getLights() const;
         std::shared_ptr<AmbientLight> getAmbientLight() const;

         void setBackground(const Background& background);
         void buildBackground();
         bool hasBackground() const;
         const std::shared_ptr<Background> getBackground() const;

         void setParam(const std::string& key, const ParamSet& value);
         ParamSet getParam(const std::string& key) const;
         const ParamSets& getParams() const;

         void include(const Scene& other, const Transform& transform);

         void prepareAggregate(const Transform& transform);
         void prepareAggregate();

         void defineNewObject(std::string name);
         void saveNewObject();
   };
}

#endif // !SCENE_HPP