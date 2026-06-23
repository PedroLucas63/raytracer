#include "Core/CTMStack.hpp"

namespace raytracer {

CTMStack::CTMStack()
    : _current(Matrix4x4::identity())
{}

void CTMStack::push() {
    _stack.push(_current);
}

void CTMStack::pop() {
    if (_stack.empty())
        throw std::runtime_error("CTMStack::pop(): stack is empty");
    _current = _stack.top();
    _stack.pop();
}

void CTMStack::apply(const Matrix& m) {
    _current = _current * m;
}

void CTMStack::reset() {
    _current = Matrix4x4::identity();
}

const Matrix& CTMStack::current() const {
    return _current;
}

Matrix CTMStack::currentInverse() const {
    return _current.inverse();
}

bool CTMStack::empty() const {
    return _stack.empty();
}

size_t CTMStack::depth() const {
    return _stack.size();
}

} // namespace raytracer
