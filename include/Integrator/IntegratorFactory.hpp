#ifndef INTEGRATOR_FACTORY_HPP
#define INTEGRATOR_FACTORY_HPP

#include "Scene/Scene.hpp"
#include "Integrator/Integrator.hpp"
#include <memory>

namespace raytracer {
   class IntegratorFactory {
      private:
         IntegratorFactory() = default;
         ~IntegratorFactory() = default;

         static std::string getAndValidateIntegratorType(const ParamSets& params);
      public:
         static std::shared_ptr<Integrator> create(const ParamSets& params);
   };
}

#endif // !INTEGRATOR_FACTORY_HPP