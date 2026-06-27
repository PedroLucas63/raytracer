#pragma once

#include "Math/Transform.hpp"
#include <stack>
#include <stdexcept>
#include <memory>

namespace raytracer {

    class CTMStack {
    private:
        std::shared_ptr<Transform> _current;
        std::stack<std::shared_ptr<Transform>>  _transforms;

    public:
        CTMStack();

        void push();
        void pop();
        void apply(const Transform& t);
        void reset();

        const std::shared_ptr<Transform>& current() const;

        bool   empty() const;
        size_t depth() const;
    };

} // namespace raytracer