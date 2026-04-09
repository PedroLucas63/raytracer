#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>
#include "Objects/Primitive.hpp"
// #include "PrimitiveFactory.hpp"
#include "Objects/Material.hpp"
#include "Objects/MaterialFactory.hpp"
#include <unordered_map>



namespace raytracer {
   using ParamSets = std::unordered_map<std::string, ParamSet>;

   class Scene {
      private:
         std::vector<std::shared_ptr<Primitive>> _primitives;
         std::unordered_map<std::string, std::shared_ptr<Material>> _materialMap;
         ParamSets _params;
         
      public:
         Scene() = default;
         ~Scene() = default;

         void addPrimitive(const std::shared_ptr<Primitive>& primitive);
         void addMaterial(const std::shared_ptr<Material>& material);
         const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const;

         Material* getMaterialAt(const std::string& name) const;
         void setParam(const std::string& key, const ParamSet& value);
         ParamSet getParam(const std::string& key) const;
         

   };
   //set params e get params
}

#endif // !SCENE_HPP