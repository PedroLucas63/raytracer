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
   CTMStack Api::_currCTM;
   std::unordered_map<std::string, Matrix> Api::_namedCoordSystem;
   GraphicsState Api::_currGS;
   std::stack<GraphicsState> Api::_savedGS;
   std::stack<Matrix> Api::_savedTM;
   std::vector<std::shared_ptr<const Transform>> Api::_transformationCache;

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
      _currCTM.reset();

      std::unordered_map<std::string, std::function<void(Scene&, const ParamSet&)>> handlers;

      handlers["world_end"] = [&](Scene&, const ParamSet&) { 
         generate(); 
      };

      handlers["render_again"] = [&](Scene&, const ParamSet&) {
         generate(); 
      };

      handlers["identity"] = [&](Scene&, const ParamSet&) {
         _currCTM.reset();
      };

      handlers["translate"] = [&](Scene&, const ParamSet& ps) {
         Vector3 v = ps.retrieve<Vector3>("value");
         _currCTM.apply(translation(
            static_cast<float>(v.getX()),
            static_cast<float>(v.getY()),
            static_cast<float>(v.getZ())
         ));
      };

      handlers["scale"] = [&](Scene&, const ParamSet& ps) {
         Vector3 v = ps.retrieve<Vector3>("value");
         _currCTM.apply(scale(
            static_cast<float>(v.getX()),
            static_cast<float>(v.getY()),
            static_cast<float>(v.getZ())
         ));
      };

      handlers["rotate"] = [&](Scene&, const ParamSet& ps) {
         Vector3 axis = ps.retrieve<Vector3>("axis");
         float angle = ps.retrieve<float>("angle");
         _currCTM.apply(rotation(axis, angle));
      };

      handlers["save_coord_system"] = [&](Scene&, const ParamSet& ps) {
         std::string name = ps.retrieve<std::string>("name");
         _namedCoordSystem[name] = _currCTM.current();
      };

      handlers["restore_coord_system"] = [&](Scene&, const ParamSet& ps) {
         std::string name = ps.retrieve<std::string>("name");
         auto it = _namedCoordSystem.find(name);
         if (it == _namedCoordSystem.end())
            throw std::runtime_error("restore_coord_system: unknown coord system '" + name + "'");
         // Replace the entire CTM stack state with the saved matrix
         _currCTM = CTMStack{};   // reset to fresh stack
         _currCTM.apply(it->second);
      };

      handlers["push_CTM"] = [&](Scene&, const ParamSet&) {
         _currCTM.push();
      };

      handlers["pop_CTM"] = [&](Scene&, const ParamSet&) {
         _currCTM.pop();
      };

      handlers["push_GS"] = [&](Scene&, const ParamSet&) {
         _savedGS.push(_currGS);
         _currCTM.push(); 
      };

      handlers["pop_GS"] = [&](Scene&, const ParamSet&) {
         if (_savedGS.empty())
            throw std::runtime_error("pop_GS: graphics-state stack is empty");
         _currGS = _savedGS.top();
         _savedGS.pop();
         _currCTM.pop();
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
      _currCTM  = CTMStack();
      _namedCoordSystem.clear();
      _currGS  = GraphicsState();
      while (!_savedGS.empty()) _savedGS.pop();
      while (!_savedTM.empty()) _savedTM.pop();
      _transformationCache.clear();
   }


   void Api::getCurrentTransform(const Transform** objToWorld, const Transform** worldToObj, bool* flipNormals) {
      Matrix m = _currCTM.current();
      Matrix mInv = _currCTM.currentInverse();

      const Transform* t_direct = nullptr;
      const Transform* t_inverse = nullptr;

      // Find or create direct
      for (const auto& t : _transformationCache) {
         if (t->getMatrix() == m) {
            t_direct = t.get();
            break;
         }
      }
      if (!t_direct) {
         auto t = std::make_shared<Transform>(m, mInv);
         _transformationCache.push_back(t);
         t_direct = t.get();
      }

      // Find or create inverse
      for (const auto& t : _transformationCache) {
         if (t->getMatrix() == mInv) {
            t_inverse = t.get();
            break;
         }
      }
      if (!t_inverse) {
         auto t = std::make_shared<Transform>(mInv, m);
         _transformationCache.push_back(t);
         t_inverse = t.get();
      }

      *objToWorld = t_direct;
      *worldToObj = t_inverse;
      if (flipNormals) {
         *flipNormals = _currGS.flipNormals;
      }
   }

} // namespace raytracer