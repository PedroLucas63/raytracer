#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>
#include "Scene/Background/Background.hpp"
#include "Objects/Primitive.hpp"
#include "Objects/Materials/Material.hpp"
#include "Objects/Materials/MaterialFactory.hpp"
#include <unordered_map>

namespace raytracer {
   using ParamSets = std::unordered_map<std::string, ParamSet>;

   class Scene {
      private:
         std::vector<std::shared_ptr<Primitive>> _primitives;
         std::unordered_map<std::string, std::shared_ptr<Material>> _materialMap;
         std::shared_ptr<Material> _lastMaterial = nullptr;

         std::shared_ptr<Background> _background;

         ParamSets _params;
         
      public:
         Scene() = default;
         ~Scene() = default;
         Scene& operator=(const Scene& other);

         void addPrimitive(const std::shared_ptr<Primitive>& primitive);
         const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const;

         void addMaterial(const std::shared_ptr<Material>& material);
         std::shared_ptr<Material> getMaterialAt(const std::string& name) const;
         std::shared_ptr<Material> getLastMaterial() const { return _lastMaterial; }

         void setBackground(const Background& background);
         void buildBackground();
         bool hasBackground() const;
         const std::shared_ptr<Background> getBackground() const;

         void setParam(const std::string& key, const ParamSet& value);
         ParamSet getParam(const std::string& key) const;
         const ParamSets& getParams() const;

         void include(const Scene& other);
   };
}

#endif // !SCENE_HPP