#ifndef RAYTRACER_API_HPP
#define RAYTRACER_API_HPP

#include "Utils/RunningOptions.hpp"
#include "Core/ParamSet.hpp"
#include "Parser/ParserScene.hpp"
#include "Scene/Scene.hpp"

namespace raytracer {
   class Api {

      std::unordered_map<std::string, std::function<void()>> handlers;
      
      private:
         static RunningOptions _options;
         static Scene _scene;

         Api() = default;
         ~Api() = default;

         static void generate();
      public:
         static void initEngine(const RunningOptions& options);
         static void run();
         static void cleanUp();         
   };
}

#endif // !RAYTRACER_API_HPP