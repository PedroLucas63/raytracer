#include "Core/CTMStack.hpp"
#include "Utils/TensorUtils.hpp"

namespace raytracer {

CTMStack::CTMStack()
    : _current(identity()),
      _currentInverse(identity())
{}

void CTMStack::push() {
    _transforms.push(_current);
    _inverses.push(_currentInverse);
}

void CTMStack::pop() {
    if (_transforms.empty())
        throw std::runtime_error("CTMStack::pop(): stack is empty");

    _current = _transforms.top();
    _transforms.pop();
    _currentInverse = _inverses.top();
    _inverses.pop();
}

void CTMStack::apply(const Tensor<double>& m) {
    _current = compose(_current, m);
    _currentInverse = _currentInverse.inverse();
}

void CTMStack::reset() {
    _current = identity();
    _currentInverse = identity();

    while (!_transforms.empty()) {
        _transforms.pop();
        _inverses.pop();
    }
}

const Tensor<double>& CTMStack::current() const {
    return _current;
}

Tensor<double> CTMStack::currentInverse() const {
    return _currentInverse;
}

bool CTMStack::empty() const {
    return _transforms.empty();
}

size_t CTMStack::depth() const {
    return _transforms.size();
}

} // namespace raytracer
