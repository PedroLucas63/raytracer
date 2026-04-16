#include "Objects/Materials/Material.hpp"

namespace raytracer {
   Material::Material(const ParamSet& params) {
      bool anonymous = !params.has("name");
      
      std::string name;
      if (anonymous) {
         name = "__material_" + std::to_string(reinterpret_cast<std::uintptr_t>(this));
      } else {
         name = params.retrieve<std::string>("name");
      }

      if (name.starts_with("__") && !anonymous) {
         throw std::invalid_argument("Material name cannot start with '__' unless it's anonymous");
      }

      _name = name;
      _annonymous = anonymous;
   }
}