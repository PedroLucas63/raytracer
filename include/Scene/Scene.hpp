#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>
#include "Objects/Primitive.hpp"

namespace raytracer {
   class Scene {
      private:
         std::vector<std::shared_ptr<Primitive>> _primitives;
         
      public:
         Scene() = default;
         ~Scene() = default;

         void addPrimitive(const std::shared_ptr<Primitive>& primitive);
         const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const;
   };
}

#endif // !SCENE_HPP