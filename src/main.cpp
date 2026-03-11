#include <print>
#include <numeric>
#include "Utils/ImageUtils.hpp"
#include "Utils/NetpbmUtils.hpp"
#include "Images/Image.hpp"
#include "Images/Background.hpp"

using namespace raytracer;

int main(int, char**) {
    std::println("Hello, Raytracer!");
    
    auto bg_solid = Background::solid(PIXEL_RED);
    auto bg_horizontal = Background::horizontalGradient(PIXEL_GREEN, PIXEL_BLUE);
    auto bg_vertical = Background::verticalGradient(PIXEL_BLACK, PIXEL_WHITE);
    auto bg_diagonal_tlbr = Background::diagonalGradientTLBR(PIXEL_CYAN, PIXEL_MAGENTA);
    auto bg_diagonal_trbl = Background::diagonalGradientTRBL(PIXEL_YELLOW, PIXEL_RED);

    save_image(bg_solid.toImage(512, 256), "background_solid.png", ImageType::PNG);
    save_image(bg_horizontal.toImage(512, 256), "background_horizontal.bmp", ImageType::BMP);
    save_image(bg_vertical.toImage(512, 256), "background_vertical.tga", ImageType::TGA);
    save_image(bg_diagonal_tlbr.toImage(512, 256), "background_diagonal_tlbr.jpg", ImageType::JPG);
    netpbm::saveImage(bg_diagonal_trbl.toImage(512, 256), "background_diagonal_trbl.ppm", netpbm::NetpbmType::P3);

    return EXIT_SUCCESS;
}
