#ifndef RAYTRACER_API_HPP
#define RAYTRACER_API_HPP

#include "Utils/RunningOptions.hpp"
#include "Core/ParamSet.hpp"

namespace raytracer {
   class Api {
      private:
         static RunningOptions _options;

         Api() = default;
         ~Api() = default;

         static void render();
      public:
         static void initEngine(const RunningOptions& options);
         static void run();
         static void cleanUp();
         static void film(const raytracer::ParamSet& ps);
         
   };
}

#endif // !RAYTRACER_API_HPP