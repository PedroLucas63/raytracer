#include "Utils/RunningOptions.hpp"
#include <print>

int main(int argc, char** argv) {
    auto options = raytracer::RunningOptions{};

    if (!options.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    if (options.getCropwindow()) {
        const auto& [x0, x1, y0, y1] = *options.getCropwindow();
        std::println("Crop window: ({}, {}, {}, {})", x0, x1, y0, y1);
    }
    std::println("Quick render: {}", options.isQuick());
    std::println("Output file: {}", options.getOutput());
    std::println("Input scene file: {}", options.getInputSceneFile());

    return EXIT_SUCCESS;
}
