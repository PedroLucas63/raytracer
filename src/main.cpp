#include "Utils/RunningOptions.hpp"
#include "Api/Api.hpp"

int main(int argc, char** argv) {
    auto options = raytracer::RunningOptions{};

    if (!options.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    raytracer::Api::initEngine(options);
    raytracer::Api::run();
    raytracer::Api::cleanUp();

    return EXIT_SUCCESS;
}
