#ifndef RAYTRACER_API_HPP
#define RAYTRACER_API_HPP

#include "Utils/RunningOptions.hpp"
#include "Core/ParamSet.hpp"
#include "Core/CTMStack.hpp"
#include "Parser/ParserScene.hpp"
#include "Scene/Scene.hpp"
#include "Math/Transform.hpp"
#include "Core/GraphicsState.hpp"

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

namespace raytracer {
    class Api {
    public:
        static void initEngine(const RunningOptions& options);
        static void run();
        static void cleanUp();

        static void getCurrentTransform(std::shared_ptr<Transform>* objToWorld, bool* flipNormals);

    private:
        Api()  = default;
        ~Api() = default;
        static void generate();
        static RunningOptions _options;
        static CTMStack _currCTM;
        static std::unordered_map<std::string, std::shared_ptr<Transform>> _namedCoordSystem;
        static GraphicsState _graphicsState;
    };

} // namespace raytracer

#endif // !RAYTRACER_API_HPP