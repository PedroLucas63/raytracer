#pragma once

#include "Math/Tensor/Tensor.hpp"
#include <stack>
#include <stdexcept>

namespace raytracer {
    class CTMStack {
    private:
        Tensor<double> _current;
        Tensor<double> _currentInverse;
        std::stack<Tensor<double>>  _transforms;
        std::stack<Tensor<double>>  _inverses;

    public:
        CTMStack();

        void push();
        void pop();
        void apply(const Tensor<double>& m);
        void reset();

        const Tensor<double>& current() const;
        Tensor<double> currentInverse() const;

        bool   empty() const;
        size_t depth() const;
    };

} // namespace raytracer