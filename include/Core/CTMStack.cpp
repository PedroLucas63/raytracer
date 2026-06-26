#include "Core/CTMStack.hpp"

namespace raytracer {

CTMStack::CTMStack()
    : _current(Matrix4x4::identity()),
      _currentInverse(Matrix4x4::identity())
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

void CTMStack::apply(const Matrix& m) {
    _current = _current * m;
    _currentInverse = _currentInverse.inverse();
}

void CTMStack::reset() {
    _current = Matrix4x4::identity();
    _currentInverse = Matrix4x4::identity();
}

const Matrix& CTMStack::current() const {
    return _current;
}

Matrix CTMStack::currentInverse() const {
    return _currentInverse;
}

bool CTMStack::empty() const {
    return _transforms.empty();
}

size_t CTMStack::depth() const {
    return _transforms.size();
}

} // namespace raytracer
