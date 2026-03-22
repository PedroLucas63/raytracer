#ifndef RAYTRACER_API_HPP
#define RAYTRACER_API_HPP

#include "Utils/RunningOptions.hpp"
#include "Core/ParamSet.hpp"
#include "Parser/ParserScene.hpp"

namespace raytracer {
   class Api {
      private:
         static RunningOptions _options;
         static ParamSets _sceneData;

         Api() = default;
         ~Api() = default;

         static void render();
         
      public:
         static void initEngine(const RunningOptions& options);
         static void run();
         static void cleanUp();         
   };
}

#endif // !RAYTRACER_API_HPP