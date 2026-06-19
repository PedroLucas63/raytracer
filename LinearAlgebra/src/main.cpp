#include <iostream>
#include <vector>
#include "../include/Tensor/Tensor.hpp"

using namespace linalg;

int main(int, char**){
    auto tensor = linalg::Tensor<float>::from_data({
        {1, 3, 2, -5},
        {1, 3, 0, 4},
        {2, -4, 1, 5},
        {-2, 0, 1, 8},
    });

    std::cout << "Original tensor:\n";
    std::cout << tensor << "\n";

    std::cout << "Inverse tensor:\n";
    std::cout << tensor.inverse() << "\n";
}