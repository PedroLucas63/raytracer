#include "Integrator/IntegratorFactory.hpp"
#include "Integrator/RayCastIntegrator.hpp"
#include "Integrator/DepthMapIntegrator.hpp"
#include "Integrator/NormalMapIntegrator.hpp"


namespace raytracer {
   std::string IntegratorFactory::getAndValidateIntegratorType(const ParamSets& params) {
      auto it = params.find("integrator");
      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'integrator' parameters.");
      
      const ParamSet& integratorParams = it->second;
         
      if (!integratorParams.has("type"))
         throw std::invalid_argument("Integrator parameters must include 'type'.");

      std::string type = integratorParams.retrieve<std::string>("type");
      if (type != "flat" && type != "depth_map" && type != "normal_map")
         throw std::invalid_argument("Unsupported integrator type: " + type);

      return type;
   }

   std::shared_ptr<Integrator> IntegratorFactory::create(const ParamSets& params) {
      std::string type = getAndValidateIntegratorType(params);
      const ParamSet& integratorParams = params.at("integrator");

      if (type == "flat") {
         return std::make_shared<RayCastIntegrator>();
      } 
      else if (type == "depth_map") {
         RGBColor nearColor = integratorParams.retrieveOrDefault("near_color", RGBColor(255, 255, 255));
         RGBColor farColor  = integratorParams.retrieveOrDefault("far_color",  RGBColor(0, 0, 0));
         float zmin = integratorParams.retrieveOrDefault("zmin", 0.0f);
         float zmax = integratorParams.retrieveOrDefault("zmax", std::numeric_limits<float>::max());
         return std::make_shared<DepthMapIntegrator>(nearColor, farColor, zmin, zmax);
 
      } else if (type == "normal_map") {
         return std::make_shared<NormalMapIntegrator>();
      } else {
         throw std::invalid_argument("Unsupported integrator type: " + type);
      }
   }
}