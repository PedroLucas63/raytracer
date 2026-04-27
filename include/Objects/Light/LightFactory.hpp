#ifndef LIGHT_FACTORY_HPP
#define LIGHT_FACTORY_HPP

#include "Objects/Light/Light.hpp"
#include "Core/ParamSet.hpp"
#include <string>
#include <memory>

namespace raytracer {
   class LightFactory {
      private:
         LightFactory() = default;
         ~LightFactory() = default;

         static std::string getType(const ParamSet& params);
         static bool validateType(const std::string& type);

      public:
         static std::shared_ptr<Light> create(const ParamSet& params);
   };
}

#endif // !LIGHT_FACTORY_HPP