#include "Integrator/IntegratorFactory.hpp"
#include "Integrator/RayCastIntegrator.hpp"

namespace raytracer {
   std::string IntegratorFactory::getAndValidateIntegratorType(const ParamSets& params) {
      auto it = params.find("integrator");
      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'integrator' parameters.");
      
      const ParamSet& integratorParams = it->second;
         
      if (!integratorParams.has("type"))
         throw std::invalid_argument("Integrator parameters must include 'type'.");

      std::string type = integratorParams.retrieve<std::string>("type");
      if (type != "flat")
         throw std::invalid_argument("Unsupported integrator type: " + type);

      return type;
   }

   std::shared_ptr<Integrator> IntegratorFactory::create(const ParamSets& params) {
      std::string type = getAndValidateIntegratorType(params);

      if (type == "flat") {
         return std::make_shared<RayCastIntegrator>();
      } else {
         throw std::invalid_argument("Unsupported integrator type: " + type);
      }
   }
}