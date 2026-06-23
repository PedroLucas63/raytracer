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
   Scene Api::_scene;
   CTMStack Api::curr_TM;
   std::unordered_map<std::string, Matrix> Api::named_coord_system;
   GraphicsState Api::curr_GS;
   std::stack<GraphicsState> Api::saved_GS;
   std::stack<Matrix> Api::saved_TM;

   void Api::generate() {
      auto integrator = IntegratorFactory::create(_scene.getParams());
      integrator->render(_scene);

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
      curr_TM.reset();

      std::unordered_map<std::string, std::function<void(Scene&, const ParamSet&)>> handlers;

      handlers["world_end"] = [&](Scene&, const ParamSet&) { 
         generate(); 
      };

      handlers["render_again"] = [&](Scene&, const ParamSet&) {
         generate(); 
      };

      handlers["identity"] = [&](Scene&, const ParamSet&) {
         curr_TM.reset();
      };

      handlers["translate"] = [&](Scene&, const ParamSet& ps) {
         Vector3 v = ps.retrieve<Vector3>("value");
         curr_TM.apply(mat4_translation(
            static_cast<float>(v.getX()),
            static_cast<float>(v.getY()),
            static_cast<float>(v.getZ())
         ));
      };

      handlers["scale"] = [&](Scene&, const ParamSet& ps) {
         Vector3 v = ps.retrieve<Vector3>("value");
         curr_TM.apply(mat4_scale(
            static_cast<float>(v.getX()),
            static_cast<float>(v.getY()),
            static_cast<float>(v.getZ())
         ));
      };

      handlers["rotate"] = [&](Scene&, const ParamSet& ps) {
         Vector3 axis  = ps.retrieve<Vector3>("axis");
         float   angle = ps.retrieve<float>("angle");
         curr_TM.apply(mat4_rotation(axis, angle));
      };

      handlers["save_coord_system"] = [&](Scene&, const ParamSet& ps) {
         std::string name = ps.retrieve<std::string>("name");
         named_coord_system[name] = curr_TM.current();
      };

      handlers["restore_coord_system"] = [&](Scene&, const ParamSet& ps) {
         std::string name = ps.retrieve<std::string>("name");
         auto it = named_coord_system.find(name);
         if (it == named_coord_system.end())
            throw std::runtime_error("restore_coord_system: unknown coord system '" + name + "'");
         // Replace the entire CTM stack state with the saved matrix
         curr_TM = CTMStack{};   // reset to fresh stack
         curr_TM.apply(it->second);
      };

      handlers["push_CTM"] = [&](Scene&, const ParamSet&) {
         curr_TM.push();
      };

      handlers["pop_CTM"] = [&](Scene&, const ParamSet&) {
         curr_TM.pop();
      };

      handlers["push_GS"] = [&](Scene&, const ParamSet&) {
         saved_GS.push(curr_GS);
         curr_TM.push(); 
      };

      handlers["pop_GS"] = [&](Scene&, const ParamSet&) {
         if (saved_GS.empty())
            throw std::runtime_error("pop_GS: graphics-state stack is empty");
         curr_GS = saved_GS.top();
         saved_GS.pop();
         curr_TM.pop();
      };

      ParserScene::parseScene(
         _options.getInputSceneFile().c_str(),
         _scene,
         [&](Scene& sceneRef, const std::string& element, const ParamSet& ps) {
            auto it = handlers.find(element);
            if (it != handlers.end())
                  it->second(sceneRef, ps);
         }
      );
   }

   void Api::cleanUp() {
      _options = RunningOptions();
      _scene   = Scene();
      curr_TM  = CTMStack();
      named_coord_system.clear();
      curr_GS  = GraphicsState();
      while (!saved_GS.empty()) saved_GS.pop();
      while (!saved_TM.empty()) saved_TM.pop();
   }

} // namespace raytracer