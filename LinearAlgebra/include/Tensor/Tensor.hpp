#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "Storage.hpp"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <ostream>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <functional>

namespace linalg {
   template <typename T>
   class Tensor {
      private:
         class TensorInitializerList {
            public:
               std::variant<std::initializer_list<TensorInitializerList>, T> value;

               TensorInitializerList(std::initializer_list<TensorInitializerList> list) : value(list) {}
               TensorInitializerList(T val) : value(val) {}
         };

         std::shared_ptr<Storage<T>> _storage;
         std::vector<size_t> _shape;
         std::vector<size_t> _strides;
         size_t _offset;
         size_t _numel;

         static void validate_shape_or_throw(const std::vector<size_t>& shape) {
            if (shape.empty()) {
               throw std::invalid_argument("Shape cannot be empty.");
            }

            for (const auto& s : shape) {
               if (s == 0) {
                  throw std::invalid_argument("Shape cannot contain an axis of length 0.");
               }
            }
         }

         static size_t compute_numel(const std::vector<size_t>& shape) {
            return std::accumulate(shape.begin(), shape.end(), size_t(1), std::multiplies<size_t>());
         }

         static std::vector<size_t> compute_strides(const std::vector<size_t>& shape) {
            std::vector<size_t> strides(shape.size(), 1);
            size_t stride = 1;

            for (size_t i = shape.size(); i-- > 0;) {
               strides[i] = stride;
               stride *= shape[i];
            }

            return strides;
         }

         static std::shared_ptr<Storage<T>> make_storage(size_t size) {
            return std::make_shared<CpuStorage<T>>(size);
         }

         size_t compute_storage_index(const std::vector<size_t>& index) const {
            if (index.size() != _shape.size()) {
               throw std::invalid_argument("Index rank must match tensor rank.");
            }

            size_t flattened = _offset;
            for (size_t i = 0; i < index.size(); ++i) {
               if (index[i] >= _shape[i]) {
                  throw std::out_of_range(
                     "Index out of bounds for axis " + std::to_string(i) +
                     " with size " + std::to_string(_shape[i])
                  );
               }

               flattened += index[i] * _strides[i];
            }

            return flattened;
         }

         static std::vector<size_t> resolve_shape_from_initializer(const TensorInitializerList& initializer) {
            std::vector<size_t> shape;

            if (std::holds_alternative<std::initializer_list<TensorInitializerList>>(initializer.value)) {
               const auto& list = std::get<std::initializer_list<TensorInitializerList>>(initializer.value);

               if (list.size() == 0) {
                  throw std::invalid_argument("Initializer list cannot be empty.");
               }

               shape.push_back(list.size());

               auto sub_shape = resolve_shape_from_initializer(*list.begin());
               shape.insert(shape.end(), sub_shape.begin(), sub_shape.end());
            }

            return shape;
         }

         static std::vector<T> get_data_from_initializer(const TensorInitializerList& initializer) {
            std::vector<T> data;

            if (std::holds_alternative<std::initializer_list<TensorInitializerList>>(initializer.value)) {
               const auto& list = std::get<std::initializer_list<TensorInitializerList>>(initializer.value);

               for (const auto& item : list) {
                  auto sub_data = get_data_from_initializer(item);
                  data.insert(data.end(), sub_data.begin(), sub_data.end());
               }
            } else {
               data.push_back(std::get<T>(initializer.value));
            }

            return data;
         }

         Tensor(
            std::shared_ptr<Storage<T>> storage,
            std::vector<size_t> shape,
            std::vector<size_t> strides,
            size_t offset,
            size_t numel
         )
            : _storage(std::move(storage)),
              _shape(std::move(shape)),
              _strides(std::move(strides)),
              _offset(offset),
              _numel(numel)
         {
            validate_shape_or_throw(_shape);
         }

         template <typename Func>
         void for_each_index(Func&& func) const {
            std::vector<size_t> idx(_shape.size(), 0);

            while (true) {
               func(idx);

               size_t dim = _shape.size();
               while (dim > 0) {
                  --dim;
                  ++idx[dim];

                  if (idx[dim] < _shape[dim]) {
                     break;
                  }

                  idx[dim] = 0;

                  if (dim == 0) {
                     return;
                  }
               }
            }
         }


         T& at(const std::vector<size_t>& index) {
            auto idx = compute_storage_index(index);

            if (idx >= _storage->size()) {
               throw std::out_of_range("Flattened index out of bounds.");
            }

            return _storage->at(idx);
         }

      public:
         explicit Tensor(const std::vector<size_t>& shape) {
            validate_shape_or_throw(shape);

            _shape = shape;
            _strides = compute_strides(shape);
            _numel = compute_numel(shape);
            _offset = 0;
            _storage = make_storage(_numel);
         }

         static Tensor<T> from_data(
            const TensorInitializerList& initializer
         ) {
            auto shape = resolve_shape_from_initializer(initializer);
            auto data = get_data_from_initializer(initializer);

            Tensor<T> tensor(shape);
            std::copy(data.begin(), data.end(), tensor._storage->data());
            return tensor;
         }

         static Tensor<T> full(
            const std::vector<size_t>& shape,
            const T& value
         ) {
            Tensor<T> tensor(shape);
            std::fill_n(tensor._storage->data(), tensor._numel, value);
            return tensor;
         }

         static Tensor<T> zeros(const std::vector<size_t>& shape) {
            return full(shape, T(0));
         }

         static Tensor<T> ones(const std::vector<size_t>& shape) {
            return full(shape, T(1));
         }

         static Tensor<T> eye(size_t n) {
            if (n == 0) {
               throw std::invalid_argument("Size of the identity matrix must be greater than 0.");
            }

            Tensor<T> tensor(std::vector<size_t>{n, n});
            for (size_t i = 0; i < n; ++i) {
               tensor._storage->at(i * (n + 1)) = T(1);
            }

            return tensor;
         }

         static Tensor<T> random_int(
            const std::vector<size_t>& shape,
            T min = std::numeric_limits<T>::min(),
            T max = std::numeric_limits<T>::max()
         ) {
            static_assert(std::integral<T>, "random_int() requires an integral tensor type.");

            if (min > max) {
               throw std::invalid_argument("min must be less than or equal to max.");
            }

            Tensor<T> tensor(shape);
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<T> dist(min, max);

            for (size_t i = 0; i < tensor._numel; ++i) {
               tensor._storage->at(i) = dist(rng);
            }

            return tensor;
         }

         static Tensor<T> random_float(
            const std::vector<size_t>& shape,
            T min = std::numeric_limits<T>::lowest(),
            T max = std::numeric_limits<T>::max()
         ) {
            static_assert(std::floating_point<T>, "random_float() requires a floating-point tensor type.");

            if (min > max) {
               throw std::invalid_argument("min must be less than or equal to max.");
            }

            Tensor<T> tensor(shape);
            std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<T> dist(min, max);

            for (size_t i = 0; i < tensor._numel; ++i) {
               tensor._storage->at(i) = dist(rng);
            }

            return tensor;
         }

         std::vector<size_t> shape() const { return _shape; }
         size_t ndim() const { return _shape.size(); }
         std::vector<size_t> strides() const { return _strides; }
         size_t offset() const { return _offset; }
         size_t numel() const { return _numel; }

         bool is_contiguous() const {
            size_t stride = 1;

            for (size_t i = _shape.size(); i-- > 0;) {
               if (_strides[i] != stride) {
                  return false;
               }
               stride *= _shape[i];
            }

            return true;
         }

         T at(const std::vector<size_t>& index) const {
            auto idx = compute_storage_index(index);

            if (idx >= _storage->size()) {
               throw std::out_of_range("Flattened index out of bounds.");
            }

            return _storage->at(idx);
         }

         Tensor<T> reshape(const std::vector<size_t>& new_shape) const {
            validate_shape_or_throw(new_shape);

            const size_t new_numel = compute_numel(new_shape);
            if (new_numel != _numel) {
               throw std::invalid_argument("Total number of elements must remain unchanged during reshape.");
            }

            auto tensor = contiguous();
            auto new_strides = compute_strides(new_shape);

            return Tensor<T>(
               tensor._storage,
               new_shape,
               new_strides,
               tensor._offset,
               tensor._numel
            );
         }

         Tensor<T> flatten() const {
            return reshape({_numel});
         }

         Tensor<T> squeeze() const {
            std::vector<size_t> new_shape;
            for (const auto& dim : _shape) {
               if (dim != 1) {
                  new_shape.push_back(dim);
               }
            }

            if (new_shape.empty()) {
               new_shape.push_back(1);
            }

            return reshape(new_shape);
         }

         Tensor<T> unsqueeze(size_t axis = 0) const {
            if (axis > _shape.size()) {
               throw std::out_of_range("Axis out of bounds for unsqueeze operation.");
            }

            std::vector<size_t> new_shape = _shape;
            new_shape.insert(new_shape.begin() + axis, 1);
            return reshape(new_shape);
         }

         Tensor<T> permute(std::vector<size_t> axes) const {
            if (axes.size() != _shape.size()) {
               throw std::invalid_argument("Number of axes must match tensor dimensions.");
            }

            std::vector<size_t> new_shape(_shape.size());
            std::vector<size_t> new_strides(_shape.size());
            std::vector<bool> seen(_shape.size(), false);

            for (size_t i = 0; i < axes.size(); ++i) {
               if (axes[i] >= _shape.size()) {
                  throw std::out_of_range("Axis index out of bounds for permute operation.");
               }
               if (seen[axes[i]]) {
                  throw std::invalid_argument("Duplicate axis index in permute operation.");
               }

               seen[axes[i]] = true;
               new_shape[i] = _shape[axes[i]];
               new_strides[i] = _strides[axes[i]];
            }

            return Tensor<T>(_storage, new_shape, new_strides, _offset, _numel);
         }

         Tensor<T> transpose() const {
            if (_shape.size() != 2) {
               throw std::invalid_argument("Transpose requires exactly 2 dimensions.");
            }

            std::vector<size_t> axes(ndim());
            std::iota(axes.begin(), axes.end(), 0);
            std::swap(axes[0], axes[1]);
            return permute(axes);
         }

         Tensor<T> transpose(size_t axis1, size_t axis2) const {
            if (_shape.size() < 2) {
               throw std::invalid_argument("Transpose requires at least 2 dimensions.");
            }

            if (axis1 >= _shape.size() || axis2 >= _shape.size()) {
               throw std::out_of_range("Axis index out of bounds for transpose operation.");
            }

            if (axis1 == axis2) {
               throw std::invalid_argument("Axis indices must be different for transpose operation.");
            }

            std::vector<size_t> axes(ndim());
            std::iota(axes.begin(), axes.end(), 0);
            std::swap(axes[axis1], axes[axis2]);
            return permute(axes);
         }

         Tensor<T> clone() const {
            return contiguous();
         }

         Tensor<T> slice(const std::vector<std::pair<size_t, size_t>>& slices) const {
            if (slices.size() > _shape.size()) {
               throw std::invalid_argument("Number of slices cannot exceed tensor dimensions.");
            }

            std::vector<size_t> new_shape;
            size_t new_offset = _offset;

            for (size_t i = 0; i < slices.size(); ++i) {
               size_t start = slices[i].first;
               size_t end = slices[i].second;

               if (start >= end || end > _shape[i]) {
                  throw std::out_of_range("Slice indices out of bounds for axis " + std::to_string(i));
               }

               new_shape.push_back(end - start);
               new_offset += start * _strides[i];
            }

            for (size_t i = slices.size(); i < _shape.size(); ++i) {
               new_shape.push_back(_shape[i]);
            }

            const auto new_numel = compute_numel(new_shape);
            return Tensor<T>(_storage, new_shape, _strides, new_offset, new_numel);
         }

         std::vector<size_t> unravel_index(size_t linear) const {
            std::vector<size_t> idx(_shape.size());

            for (size_t i = 0; i < _shape.size(); ++i) {
               idx[i] = linear / _strides[i];
               linear %= _strides[i];
            }

            return idx;
         }

         Tensor<T> contiguous() const {
            if (is_contiguous() && _offset == 0) {
               return *this;
            }

            Tensor<T> result(_shape);

            for (size_t linear = 0; linear < _numel; ++linear) {
               auto idx = result.unravel_index(linear);
               result._storage->at(linear) = at(idx);
            }

            return result;
         }

      private:
         template <typename BinaryOp>
         Tensor<T> elementwise_binary(const Tensor<T>& other, BinaryOp op) const {
            auto lhs = contiguous();
            auto rhs = other.contiguous();

            Tensor<T> result(_shape);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = op(lhs._storage->at(i), rhs._storage->at(i));
            }

            return result;
         }

         template <typename UnaryOp>
         Tensor<T> elementwise_unary(UnaryOp op) const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = op(src._storage->at(i));
            }

            return result;
         }

      public:
         Tensor<T> operator+(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            return elementwise_binary(other, [](const T& a, const T& b) { return a + b; });
         }

         Tensor<T> operator-(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            return elementwise_binary(other, [](const T& a, const T& b) { return a - b; });
         }

         Tensor<T> operator*(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            return elementwise_binary(other, [](const T& a, const T& b) { return a - b; });
         }

         Tensor<T> operator/(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto rhs = other.contiguous();
            for (size_t i = 0; i < _numel; ++i) {
               if (rhs._storage->at(i) == T(0)) {
                  throw std::domain_error("Division by zero in tensor division.");
               }
            }

            return elementwise_binary(other, [](const T& a, const T& b) { return a / b; });
         }

         Tensor<T> dot(const Tensor<T>& other) const {
            if (_shape.size() != 2 || other._shape.size() != 2) {
               throw std::invalid_argument("Dot product requires 2D tensors.");
            }

            if (_shape[1] != other._shape[0]) {
               throw std::invalid_argument("Inner dimensions must match for dot product.");
            }

            size_t m = _shape[0];
            size_t n = _shape[1];
            size_t p = other._shape[1];

            auto lhs = contiguous();
            auto rhs = other.contiguous();

            Tensor<T> result({m, p});
            for (size_t i = 0; i < m; ++i) {
               for (size_t j = 0; j < p; ++j) {
                  T sum = T(0);
                  for (size_t k = 0; k < n; ++k) {
                     sum += lhs._storage->at(i * n + k) * rhs._storage->at(k * p + j);
                  }
                  result._storage->at(i * p + j) = sum;
               }
            }

            return result;
         }

         Tensor<T> operator+(T scalar) const {
            auto src = contiguous();

            Tensor<T> result(_shape);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = src._storage->at(i) + scalar;
            }
            return result;
         }

         Tensor<T> operator-(T scalar) const {
            auto src = contiguous();

            Tensor<T> result(_shape);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = src._storage->at(i) - scalar;
            }
            return result;
         }

         Tensor<T> operator*(T scalar) const {
            auto src = contiguous();

            Tensor<T> result(_shape);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = src._storage->at(i) * scalar;
            }
            return result;
         }

         Tensor<T> operator/(T scalar) const {
            if (scalar == T(0)) {
               throw std::domain_error("Division by zero in tensor-scalar division.");
            }

            auto src = contiguous();

            Tensor<T> result(_shape);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = src._storage->at(i) / scalar;
            }
            return result;
         }

         T sum() const {
            auto src = contiguous();
            T total = T(0);

            for (size_t i = 0; i < _numel; ++i) {
               total += src._storage->at(i);
            }

            return total;
         }

         T mean() const {
            if (_numel == 0) {
               throw std::domain_error("Cannot compute mean of an empty tensor.");
            }

            return sum() / static_cast<T>(_numel);
         }

         T min() const {
            if (_numel == 0) {
               throw std::domain_error("Cannot compute min of an empty tensor.");
            }

            auto src = contiguous();
            T minimum = src._storage->at(0);

            for (size_t i = 1; i < _numel; ++i) {
               T value = src._storage->at(i);
               if (value < minimum) {
                  minimum = value;
               }
            }

            return minimum;
         }

         T max() const {
            if (_numel == 0) {
               throw std::domain_error("Cannot compute max of an empty tensor.");
            }

            auto src = contiguous();
            T maximum = src._storage->at(0);

            for (size_t i = 1; i < _numel; ++i) {
               T value = src._storage->at(i);
               if (value > maximum) {
                  maximum = value;
               }
            }

            return maximum;
         }

         Tensor<T> abs() const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               using std::abs;
               result._storage->at(i) = abs(src._storage->at(i));
            }

            return result;
         }

         Tensor<T> sqrt() const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               T value = src._storage->at(i);
               if (value < T(0)) {
                  throw std::domain_error("Cannot compute square root of a negative value in tensor.");
               }
               result._storage->at(i) = std::sqrt(value);
            }

            return result;
         }

         Tensor<T> exp() const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::exp(src._storage->at(i));
            }

            return result;
         }

         Tensor<T> log() const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               T value = src._storage->at(i);
               if (value <= T(0)) {
                  throw std::domain_error("Cannot compute natural logarithm of a non-positive value in tensor.");
               }
               result._storage->at(i) = std::log(value);
            }

            return result;
         }

         Tensor<T> pow(T exponent) const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::pow(src._storage->at(i), exponent);
            }

            return result;
         }

         Tensor<T> clamp(T min, T max) const {
            auto src = contiguous();
            Tensor<T> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::clamp(src._storage->at(i), min, max);
            }

            return result;
         }

         bool any(std::function<bool(const T&)>& predicate) const {
            auto src = contiguous();

            for (size_t i = 0; i < _numel; ++i) {
               if (predicate(src._storage->at(i))) {
                  return true;
               }
            }

            return false;
         }

         bool all(std::function<bool(const T&)>& predicate) const {
            auto src = contiguous();

            for (size_t i = 0; i < _numel; ++i) {
               if (!predicate(src._storage->at(i))) {
                  return false;
               }
            }

            return true;
         }

         size_t count(std::function<bool(const T&)>& predicate) const {
            auto src = contiguous();
            size_t count = 0;

            for (size_t i = 0; i < _numel; ++i) {
               if (predicate(src._storage->at(i))) {
                  count++;
               }
            }

            return count;
         }

         Tensor<T> where(std::function<bool(const T&)>& predicate) const {
            auto src = contiguous();
            Tensor<bool> result({_numel});

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = predicate(src._storage->at(i));
            }

            return result.reshape(_shape);
         }

         T trace() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("Trace requires a square 2D tensor.");
            }

            auto src = contiguous();
            T total = T(0);
            size_t n = _shape[0];

            for (size_t i = 0; i < n; ++i) {
               total += src._storage->at(i * n + i);
            }

            return total;
         }

         T determinant() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("Determinant requires a square 2D tensor.");
            }

            auto src = contiguous();
            size_t n = _shape[0];

            if (n == 1) {
               return src._storage->at(0);
            } else if (n == 2) {
               return src._storage->at(0) * src._storage->at(3) - src._storage->at(1) * src._storage->at(2);
            } else {
               T det = T(0);
               for (size_t j = 0; j < n; ++j) {

                  Tensor<T> submatrix({n - 1, n - 1});
                  for (size_t i = 1; i < n; ++i) {
                     for (size_t k = 0; k < n; ++k) {
                        if (k < j) {
                           submatrix._storage->at((i - 1) * (n - 1) + k) = src._storage->at(i * n + k);
                        } else if (k > j) {
                           submatrix._storage->at((i - 1) * (n - 1) + (k - 1)) = src._storage->at(i * n + k);
                        }
                     }
                  }

                  auto sub_det = submatrix.determinant();
                  det += ((j % 2 == 0) ? src._storage->at(j) : -src._storage->at(j)) * sub_det;
               }
               return det;
            }
         }

         Tensor<T> inverse() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("Inverse requires a square 2D tensor.");
            }

            T det = determinant();
            if (det == T(0)) {
               throw std::domain_error("Matrix is singular and cannot be inverted.");
            }

            auto src = contiguous();
            size_t n = _shape[0];
            Tensor<T> result({n, n});

            if (n == 1) {
               result._storage->at(0) = T(1) / src._storage->at(0);
            } else if (n == 2) {
               result._storage->at(0) = src._storage->at(3) / det;
               result._storage->at(1) = -src._storage->at(1) / det;
               result._storage->at(2) = -src._storage->at(2) / det;
               result._storage->at(3) = src._storage->at(0) / det;
            } else {
               result = eye(n);

               for (size_t i = 0, j = 0; i < n && j < n;) {
                  // ETAPA 1
                  auto a = src.at({i, j});
                  if (a == 0) {
                     // ETAPA 1.1
                     size_t swap_row = i + 1;
                     while (swap_row < n && src.at({swap_row, j}) == 0) {
                        swap_row++;
                     }

                     if (swap_row != n) {
                        for (size_t k = 0; k < n; ++k) {
                           std::swap(src._storage->at(i * n + k), src._storage->at(swap_row * n + k));
                           std::swap(result._storage->at(i * n + k), result._storage->at(swap_row * n + k));
                        }
                        a = src.at({i, j});
                     } else {
                        // ETAPA 1.2
                        j++;
                        continue;
                     }
                  }

                  // ETAPA 2
                  for (size_t k = 0; k < n; ++k) {
                     src.at({i, k}) /= a;
                     result.at({i, k}) /= a;
                  }

                  // ETAPA 3
                  for (size_t r = 0; r < n; ++r) {
                     if (r != i) {
                        T factor = src.at({r, j});

                        for (size_t k = 0; k < n; ++k) {
                           src.at({r, k}) -= factor * src.at({i, k});
                           result.at({r, k}) -= factor * result.at({i, k});
                        }
                     }
                  }

                  // ETAPA 4
                  i++;
                  j++;
               }
            }

            return result;
         }

         Tensor<bool> operator==(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhs = contiguous();
            auto rhs = other.contiguous();
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhs._storage->at(i) == rhs._storage->at(i));
            }

            return result;
         }

         Tensor<bool> operator!=(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhs = contiguous();
            auto rhs = other.contiguous();
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhs._storage->at(i) != rhs._storage->at(i));
            }

            return result;
         }

         Tensor<bool> operator<(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhs = contiguous();
            auto rhs = other.contiguous();
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhs._storage->at(i) < rhs._storage->at(i));
            }

            return result;
         }

         Tensor<bool> operator>(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhs = contiguous();
            auto rhs = other.contiguous();
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhs._storage->at(i) > rhs._storage->at(i));
            }

            return result;
         }

         Tensor<bool> operator>=(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhs = contiguous();
            auto rhs = other.contiguous();
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhs._storage->at(i) >= rhs._storage->at(i));
            }

            return result;
         }

         Tensor<bool> operator<=(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhs = contiguous();
            auto rhs = other.contiguous();
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhs._storage->at(i) <= rhs._storage->at(i));
            }

            return result;
         }


   };

   template<typename T>
   void print_recursive(
      const Tensor<T>& tensor,
      std::vector<size_t>& indices,
      size_t dim,
      std::ostream& os,
      size_t indent = 0
   ) {
      const auto& shape = tensor.shape();

      if (shape.size() == 1) {
         os << std::string(indent * 2, ' ') << "[";
         for (size_t i = 0; i < shape[0]; ++i) {
            indices[0] = i;
            os << tensor.at(indices);
            if (i + 1 < shape[0]) {
               os << ", ";
            }
         }
         os << "]";
         return;
      }

      if (dim == shape.size() - 1) {
         os << std::string(indent * 2, ' ') << "[";

         for (size_t i = 0; i < shape[dim]; ++i) {
            indices[dim] = i;
            os << tensor.at(indices);
            if (i + 1 < shape[dim]) {
               os << ", ";
            }
         }

         os << "]";
         return;
      }

      os << std::string(indent * 2, ' ') << "[\n";

      for (size_t i = 0; i < shape[dim]; ++i) {
         indices[dim] = i;

         print_recursive(
            tensor,
            indices,
            dim + 1,
            os,
            indent + 1
         );

         if (i + 1 < shape[dim]) {
            os << ",";
         }

         os << "\n";
      }

      os << std::string(indent * 2, ' ') << "]";
   }

   template<typename T>
   void print_tensor(const Tensor<T>& tensor, std::ostream& os = std::cout) {
      if (tensor.ndim() == 0) {
         os << tensor.at({0}) << '\n';
         return;
      }

      std::vector<size_t> indices(tensor.ndim(), 0);
      print_recursive(tensor, indices, 0, os);
      os << '\n';
   }

   template<typename T>
   std::ostream& operator<<(std::ostream& os, const Tensor<T>& tensor) {
      print_tensor(tensor, os);
      return os;
   }
}

#endif // TENSOR_HPP
