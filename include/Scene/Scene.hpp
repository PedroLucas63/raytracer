#ifndef SCENE_HPP
#define SCENE_HPP


#include "Scene/Background/Background.hpp"
#include "Objects/Primitive.hpp"
#include "Objects/Materials/Material.hpp"
#include "Objects/Materials/MaterialFactory.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include "Objects/Light/Light.hpp"
#include "Objects/Light/AmbientLight.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace raytracer {
   using ParamSets = std::unordered_map<std::string, ParamSet>;

   class Scene {
      private:
         std::shared_ptr<PrimitiveList> _aggregate;
         std::unordered_map<std::string, std::shared_ptr<Material>> _materialMap;
         std::shared_ptr<Material> _lastMaterial = nullptr;

         std::vector<std::shared_ptr<Light>> _lights;
         std::shared_ptr<AmbientLight> _ambientLight = nullptr;

         std::shared_ptr<Background> _background;

         ParamSets _params;
         
      public:
         Scene(){
            _aggregate = std::make_shared<PrimitiveList>();
         }
         ~Scene() = default;
         Scene& operator=(const Scene& other);

         bool intersectWithSurfel(const Ray& r, Surfel* surfel) const;
         bool intersect(const Ray& r) const;

         void addNamedMaterial(const std::shared_ptr<Material>& material);
         void activateNamedMaterial(const std::string& name);

         void addPrimitive(const std::shared_ptr<Primitive>& primitive);
         void addMaterial(const std::shared_ptr<Material>& material);
         std::shared_ptr<Material> getMaterialAt(const std::string& name) const;
         std::shared_ptr<Material> getLastMaterial() const { return _lastMaterial; }

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

         void include(const Scene& other);
   };
}

#endif // !SCENE_HPP