#pragma once

#include "Math/Matrix.hpp"
#include <stack>
#include <stdexcept>

namespace raytracer {

    class CTMStack {
    private:
        Matrix _current;
        std::stack<Matrix>  _stack;

    public:
        CTMStack();

        void push();
        void pop();
        void apply(const Matrix& m);
        void reset();

        const Matrix& current() const;
        Matrix currentInverse() const;

        bool   empty() const;
        size_t depth() const;
    };

} // namespace raytracer