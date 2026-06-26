#ifndef RAYTRACER_API_HPP
#define RAYTRACER_API_HPP

#include "Utils/RunningOptions.hpp"
#include "Core/ParamSet.hpp"
#include "Core/CTMStack.hpp"
#include "Parser/ParserScene.hpp"
#include "Scene/Scene.hpp"
#include "Math/Transform.hpp"

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

namespace raytracer {

    // the "machine state" the API keeps while parsing a scene.
    struct GraphicsState {
        using DictOfMat = std::unordered_map<std::string, std::shared_ptr<Material>>;
        
        std::shared_ptr<Material> curr_material;
        bool flipNormals{ false };
        std::shared_ptr<DictOfMat> mats_lib;
        bool mats_lib_cloned{ false };

        GraphicsState() : mats_lib(std::make_shared<DictOfMat>()) {}
    };


    class Api {
    public:
        static void initEngine(const RunningOptions& options);
        static void run();
        static void cleanUp();

        static void getCurrentTransform(const Transform** objToWorld, const Transform** worldToObj, bool* flipNormals);

    private:
        Api()  = default;
        ~Api() = default;
        static void generate();
        static RunningOptions _options;
        static Scene _scene;
        static CTMStack _currCTM;
        static std::unordered_map<std::string, Matrix> _namedCoordSystem;
        static GraphicsState _currGS;
        static std::stack<GraphicsState> _savedGS;
        static std::stack<Matrix> _savedTM;
        static std::vector<std::shared_ptr<const Transform>> _transformationCache;
    };

} // namespace raytracer

#endif // !RAYTRACER_API_HPP