#include "Api/Api.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Background/BackgroundFactory.hpp"
#include "Utils/ProgressBar.hpp"
#include "Objects/Shapes/Sphere.hpp"
#include "Integrator/IntegratorFactory.hpp"
#include "Math/Matrix.hpp"

#include <iostream>
#include <stdexcept>

namespace raytracer {

   RunningOptions Api::_options;
   CTMStack Api::_currCTM;
   std::unordered_map<std::string, std::shared_ptr<Transform>> Api::_namedCoordSystem;
   GraphicsState Api::_graphicsState;

   void Api::generate() {
      auto scene = _graphicsState.getScene();
      auto integrator = IntegratorFactory::create(scene.getParams());
      integrator->render(scene);

      if (_options.hasOutput())
         integrator->saveImage(_options.getOutput(), _options.noOverwrite());
      else
         integrator->saveImage(_options.noOverwrite());
   }

   void Api::initEngine(const RunningOptions& options) {
         if (options.isFail())
            throw std::runtime_error("Failed to initialize engine due to invalid options.");
         _options = options;
   }

   void Api::run() {
      _currCTM.reset();

      std::unordered_map<std::string, std::function<void(GraphicsState&, const ParamSet&)>> handlers;

      handlers["world_end"] = [&](GraphicsState&, const ParamSet&) { 
         generate(); 
      };

      handlers["render_again"] = [&](GraphicsState&, const ParamSet&) {
         generate(); 
      };

      handlers["identity"] = [&](GraphicsState&, const ParamSet&) {
         _currCTM.reset();
      };

      handlers["translate"] = [&](GraphicsState&, const ParamSet& ps) {
         Vector3 v = ps.retrieve<Vector3>("value");
         _currCTM.apply(translation(v));
      };

      handlers["scale"] = [&](GraphicsState&, const ParamSet& ps) {
         Vector3 v = ps.retrieve<Vector3>("value");
         _currCTM.apply(scale(v));
      };

      handlers["rotate"] = [&](GraphicsState&, const ParamSet& ps) {
         Vector3 axis = ps.retrieve<Vector3>("axis");
         float angle = ps.retrieve<float>("angle");
         _currCTM.apply(rotation(axis, angle));
      };

      handlers["save_coord_system"] = [&](GraphicsState&, const ParamSet& ps) {
         std::string name = ps.retrieve<std::string>("name");
         _namedCoordSystem[name] = _currCTM.current();
      };

      handlers["restore_coord_system"] = [&](GraphicsState&, const ParamSet& ps) {
         std::string name = ps.retrieve<std::string>("name");
         auto it = _namedCoordSystem.find(name);
         if (it == _namedCoordSystem.end())
            throw std::runtime_error("restore_coord_system: unknown coord system '" + name + "'");

         _currCTM.reset();
         _currCTM.apply(*(it->second));
      };

      handlers["push_CTM"] = [&](GraphicsState&, const ParamSet&) {
         _currCTM.push();
      };

      handlers["pop_CTM"] = [&](GraphicsState&, const ParamSet&) {
         _currCTM.pop();
      };

      handlers["push_GS"] = [&](GraphicsState&, const ParamSet&) {
         _graphicsState.push();
         _currCTM.push(); 
      };

      handlers["pop_GS"] = [&](GraphicsState&, const ParamSet&) {
         if (_graphicsState.empty())
            throw std::runtime_error("pop_GS: graphics-state stack is empty");
         _graphicsState.pop();
         _currCTM.pop();
      };

      ParserScene::parseScene(
         _options.getInputSceneFile().c_str(),
         _graphicsState,
         [&](GraphicsState& graphicsState, const std::string& element, const ParamSet& ps) {
            auto it = handlers.find(element);
            if (it != handlers.end())
                  it->second(graphicsState, ps);
         }
      );
   }

   void Api::cleanUp() {
      _options = RunningOptions();
      _graphicsState = GraphicsState();
      _currCTM.reset();
      _namedCoordSystem.clear();
   }


   void Api::getCurrentTransform(std::shared_ptr<Transform>* objToWorld, bool* flipNormals) {
      *objToWorld = _currCTM.current();
      if (flipNormals) {
         *flipNormals = _graphicsState.getFlipNormals();
      }
   }
} // namespace raytracer