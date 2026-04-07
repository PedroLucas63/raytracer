#include "Scene.hpp"

namespace raytracer {
   void Scene::addPrimitive(const std::shared_ptr<Primitive>& primitive) {
      _primitives.push_back(primitive);
   }

   const std::vector<std::shared_ptr<Primitive>>& Scene::getPrimitives() const {
      return _primitives;
   }
}