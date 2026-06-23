#ifndef RAYTRACER_API_HPP
#define RAYTRACER_API_HPP

#include "Utils/RunningOptions.hpp"
#include "Core/ParamSet.hpp"
#include "Core/CTMStack.hpp"
#include "Parser/ParserScene.hpp"
#include "Scene/Scene.hpp"

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

namespace raytracer {

    // the "machine state" the API keeps while parsing a scene.
    struct GraphicsState {
        std::shared_ptr<Material> curr_material;
        bool flip_normals{ false };
        using DictOfMat = std::unordered_map<std::string, std::shared_ptr<Material>>;
        std::shared_ptr<DictOfMat> mats_lib;
        bool mats_lib_cloned{ false };

        GraphicsState() : mats_lib(std::make_shared<DictOfMat>()) {}
    };


    class Api {
    public:
        static void initEngine(const RunningOptions& options);
        static void run();
        static void cleanUp();

    private:
        Api()  = default;
        ~Api() = default;
        static void generate();
        static RunningOptions _options;
        static Scene _scene;
        static CTMStack curr_TM;
        static std::unordered_map<std::string, Matrix> named_coord_system;
        static GraphicsState curr_GS;
        static std::stack<GraphicsState> saved_GS;
        static std::stack<Matrix> saved_TM;
    };

} // namespace raytracer

#endif // !RAYTRACER_API_HPP