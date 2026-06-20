#include <iostream>
#include <vector>
#include "../include/Tensor/Tensor.hpp"

using namespace linalg;

int main(int, char**){
    auto tensor = linalg::Tensor<float>::from_data({
        {1, 2, 1, -3},
        {1, 1, 1, 1},
        {2, -5, 2, 1},
        {2, 2, 1, 2}
    });

    std::cout << "Original tensor:\n";
    std::cout << tensor << "\n";

    std::cout << "Inverse tensor:\n";
    std::cout << tensor.inverse() << "\n";
}