#ifndef BACKGROUND_FACTORY_HPP
#define BACKGROUND_FACTORY_HPP

#include "Scene/Background/Background.hpp"
#include "Parser/ParserScene.hpp"
#include <string>
#include <memory>

namespace raytracer {
   class BackgroundFactory {
      private:
         static std::string getAndValidateBackgroundType(const ParamSets& params);

      public:
         static std::shared_ptr<Background> create(const ParamSets& params);
   };
}

#endif // !BACKGROUND_FACTORY_HPP