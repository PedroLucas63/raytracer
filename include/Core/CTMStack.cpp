#include "Core/CTMStack.hpp"

namespace raytracer {

    CTMStack::CTMStack() : _current(std::make_shared<Transform>()) {}

    void CTMStack::push() {
        _transforms.push(_current);
    }

    void CTMStack::pop() {
        if (_transforms.empty())
            throw std::runtime_error("CTMStack::pop(): stack is empty");

        _current = _transforms.top();
        _transforms.pop();
    }

    void CTMStack::apply(const Transform& m) {
        _current = std::make_shared<Transform>((*_current) * m);
    }

    void CTMStack::reset() {
        _current = std::make_shared<Transform>();
        while(!_transforms.empty()) _transforms.pop();
    }

    const std::shared_ptr<Transform>& CTMStack::current() const {
        return _current;
    }

    bool CTMStack::empty() const {
        return _transforms.empty();
    }

    size_t CTMStack::depth() const {
        return _transforms.size();
    }

} // namespace raytracer
