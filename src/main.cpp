#include <print>
#include <numeric>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(int, char**) {
    std::println("Hello, Raytracer!");

    int width, height, channels;
    auto image = stbi_load("assets/images/panda.jpg", &width, &height, &channels, 0);
    
    if (!image) {
        std::println("Erro ao carregar imagem: {}", stbi_failure_reason());
        return 1;
    }

    std::println("Imagem carregada!");
    std::println("Width: {}", width);
    std::println("Height: {}", height);
    std::println("Channels: {}", channels);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;
            int mean =  std::accumulate(image + index, image + index + channels, 0) / channels;
            std::print("{}", mean > 128 ? '.' : '#');
        }
        std::println();
    }

    stbi_image_free(image);
    return EXIT_SUCCESS;
}
