#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "Math/Tensor/Storage.hpp"

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
#include <span>
#include <tuple>

namespace raytracer {
   template <typename T>
   class Tensor {
      private:
         class TensorInitializerList {
            public:
               std::variant<std::initializer_list<TensorInitializerList>, T> value;

               TensorInitializerList(std::initializer_list<TensorInitializerList> list) : value(list) {}
               TensorInitializerList(T val) : value(val) {}
         };

         class Iterator {
            private:
               const std::vector<size_t>& _shape;
               const std::vector<size_t>& _strides;
               std::shared_ptr<Storage<T>> _storage;
               std::vector<size_t> _index;
               size_t _offset;
               bool _end;
            public:
               Iterator(
                  const std::vector<size_t>& shape, 
                  const std::vector<size_t>& strides,
                  const std::shared_ptr<Storage<T>> storage,
                  const size_t start_offset = 0
               ) : _shape(shape), 
                  _strides(strides), 
                  _index(shape.size(), 0),
                  _storage(storage),
                  _offset(start_offset),
                  _end(shape.empty())
               {}

               size_t offset() const { return _offset; }
               bool done() const { return _end; }
               bool start() const { 
                  return !_end && std::all_of(
                     _index.begin(), 
                     _index.end(), 
                     [](size_t idx) { return idx == 0; }
                  ); 
               }

               T get() const {
                  return _storage->at(_offset);
               }

               void next() {
                  for (int dim = (int)_shape.size() - 1; dim >= 0; --dim) {
                     _index[dim]++;
                     _offset += _strides[dim];

                     if (_index[dim] < _shape[dim]) {
                        return;
                     }

                     _offset -= (_shape[dim]  - 1) * _strides[dim];
                     _index[dim] = 0;
                  }

                  _end = true;
               }

               void previous() {
                  _end = false;
                  for (int dim = (int)_shape.size() - 1; dim >= 0; --dim) {
                     if (_index[dim] == 0) {
                        _index[dim] = _shape[dim] - 1;
                        _offset += (_shape[dim] - 1) * _strides[dim];
                        continue;
                     }

                     _index[dim]--;
                     _offset -= _strides[dim];
                     return;
                  }
               }
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

         size_t compute_storage_index(std::span<const size_t> index) const {
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


         T& at(std::span<const size_t> index) {
            auto idx = compute_storage_index(index);

            if (idx >= _storage->size()) {
               throw std::out_of_range("Flattened index out of bounds.");
            }

            return _storage->at(idx);
         }

         T& at(std::initializer_list<size_t> index) {
            return at(std::span<const size_t>(index.begin(), index.size()));
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

            auto tensor = Tensor<T>::zeros({n, n});
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

         const T& at(std::span<const size_t> index) const {
            auto idx = compute_storage_index(index);

            if (idx >= _storage->size()) {
               throw std::out_of_range("Flattened index out of bounds.");
            }

            return _storage->at(idx);
         }

         const T& at(std::initializer_list<size_t> index) const {
            return at(std::span<const size_t>(index.begin(), index.size()));
         }

         const T& operator()(std::span<const size_t> index) const {
            return at(index);
         }

         const T& operator()(std::initializer_list<size_t> index) const {
            return at(index);
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

            auto it = Iterator(_shape, _strides, _storage, _offset);
            Tensor<T> result(_shape);

            for (size_t linear = 0; linear < _numel; ++linear) {
               result._storage->at(linear) = it.get();
               it.next();
            }

            return result;
         }

      private:
         template <typename BinaryOp>
         Tensor<T> elementwise_binary(const Tensor<T>& other, BinaryOp op) const {
            Tensor<T> result(_shape);

            if (this->is_contiguous() && other.is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  result._storage->at(i) = op(
                     this->_storage->at(this->_offset + i),
                     other._storage->at(other._offset + i)
                  );
               }
               return result;
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = op(
                  lhsIt.get(),
                  rhsIt.get()
               );

               lhsIt.next();
               rhsIt.next();
            }

            return result;
         }

         template <typename UnaryOp>
         Tensor<T> elementwise_unary(UnaryOp op) const {
            Tensor<T> result(_shape);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  result._storage->at(i) = op(this->_storage->at(this->_offset + i));
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = op(it.get());
               it.next();
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

            return elementwise_binary(other, [](const T& a, const T& b) { return a * b; });
         }

         Tensor<T> operator/(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto it = Iterator(other._shape, other._strides, other._storage, other._offset);
            for (size_t i = 0; i < _numel; ++i) {
               if (it.get() == T(0)) {
                  throw std::domain_error("Division by zero in tensor division.");
               }
               it.next();
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

            Tensor<T> result({m, p});

            size_t l_row_stride = _strides[0], l_col_stride = _strides[1];
            size_t r_row_stride = other._strides[0], r_col_stride = other._strides[1];
            size_t res_row_stride = result._strides[0], res_col_stride = result._strides[1];

            const T* l_data = _storage->data() + _offset;
            const T* r_data = other._storage->data() + other._offset;
            T* res_data = result._storage->data();

            for (size_t i = 0; i < m; ++i) {
               for (size_t j = 0; j < p; ++j) {
                  T sum = T(0);
                  for (size_t k = 0; k < n; ++k) {
                     T a = l_data[i * l_row_stride + k * l_col_stride];
                     T b = r_data[k * r_row_stride + j * r_col_stride];
                     sum += a * b;
                  }
                  res_data[i * res_row_stride + j * res_col_stride] = sum;
               }
            }

            return result;
         }

         Tensor<T> operator+(T scalar) const {
            return elementwise_unary([scalar](const T& a) { return a + scalar; });
         }

         Tensor<T> operator-(T scalar) const {
            return elementwise_unary([scalar](const T& a) { return a - scalar; });
         }

         Tensor<T> operator*(T scalar) const {
            return elementwise_unary([scalar](const T& a) { return a * scalar; });
         }

         Tensor<T> operator/(T scalar) const {
            if (scalar == T(0)) {
               throw std::domain_error("Division by zero in tensor-scalar division.");
            }

            return elementwise_unary([scalar](const T& a) { return a / scalar; });
         }

         T sum() const {
            T total = T(0);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  total += _storage->at(_offset + i);
               }
               return total;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               total += it.get();
               it.next();
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

            auto it = Iterator(_shape, _strides, _storage, _offset);
            T minimum = it.get();

            if (this->is_contiguous()) {
               for (size_t i = 1; i < _numel; ++i) {
                  T value = _storage->at(_offset + i);
                  if (value < minimum) {
                     minimum = value;
                  }
               }
               return minimum;
            }

            for (size_t i = 1; i < _numel; ++i) {
               it.next();
               T value = it.get();
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

            auto it = Iterator(_shape, _strides, _storage, _offset);
            T maximum = it.get();

            if (this->is_contiguous()) {
               for (size_t i = 1; i < _numel; ++i) {
                  T value = _storage->at(_offset + i);
                  if (value > maximum) {
                     maximum = value;
                  }
               }
               return maximum;
            }

            for (size_t i = 1; i < _numel; ++i) {
               it.next();
               T value = it.get();
               if (value > maximum) {
                  maximum = value;
               }
            }

            return maximum;
         }

         Tensor<T> abs() const {
            Tensor<T> result(_shape);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  result._storage->at(i) = std::abs(this->_storage->at(this->_offset + i));
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::abs(it.get());
               it.next();
            }

            return result;
         }

         Tensor<T> sqrt() const {
            Tensor<T> result(_shape);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  T value = this->_storage->at(this->_offset + i);
                  if (value < T(0)) {
                     throw std::domain_error("Cannot compute square root of a negative value in tensor.");
                  }
                  result._storage->at(i) = std::sqrt(value);
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               T value = it.get();
               if (value < T(0)) {
                  throw std::domain_error("Cannot compute square root of a negative value in tensor.");
               }
               result._storage->at(i) = std::sqrt(value);
               it.next();
            }

            return result;
         }

         Tensor<T> exp() const {
            Tensor<T> result(_shape);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  result._storage->at(i) = std::exp(this->_storage->at(this->_offset + i));
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::exp(it.get());
               it.next();
            }

            return result;
         }

         Tensor<T> log() const {
            Tensor<T> result(_shape);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  T value = this->_storage->at(this->_offset + i);
                  if (value <= T(0)) {
                     throw std::domain_error("Cannot compute natural logarithm of a non-positive value in tensor.");
                  }
                  result._storage->at(i) = std::log(value);
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               T value = it.get();
               if (value <= T(0)) {
                  throw std::domain_error("Cannot compute natural logarithm of a non-positive value in tensor.");
               }
               result._storage->at(i) = std::log(value);
               it.next();
            }

            return result;
         }

         Tensor<T> pow(T exponent) const {
            Tensor<T> result(_shape);

            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  result._storage->at(i) = std::pow(this->_storage->at(this->_offset + i), exponent);
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::pow(it.get(), exponent);
               it.next();
            }

            return result;
         }

         Tensor<T> clamp(T min, T max) const {

            if (min > max) {
               throw std::invalid_argument("min must be less than or equal to max for clamp operation.");
            }

            Tensor<T> result(_shape);
            if (this->is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  T value = this->_storage->at(this->_offset + i);
                  result._storage->at(i) = std::clamp(value, min, max);
               }
               return result;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = std::clamp(it.get(), min, max);
               it.next();
            }

            return result;
         }

         bool any(std::function<bool(const T&)>& predicate) const {
            if (is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  if (predicate(_storage->at(_offset + i))) {
                     return true;
                  }
               }
               return false;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               if (predicate(it.get())) {
                  return true;
               }
               it.next();
            }

            return false;
         }

         bool all(std::function<bool(const T&)>& predicate) const {
            if (is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  if (!predicate(_storage->at(_offset + i))) {
                     return false;
                  }
               }
               return true;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               if (!predicate(it.get())) {
                  return false;
               }
               it.next();
            }

            return true;
         }

         size_t count(std::function<bool(const T&)>& predicate) const {
            size_t count = 0;

            if (is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  if (predicate(_storage->at(_offset + i))) {
                     count++;
                  }
               }
               return count;
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               if (predicate(it.get())) {
                  count++;
               }
               it.next();
            }

            return count;
         }

         Tensor<T> where(std::function<bool(const T&)>& predicate) const {
            Tensor<bool> result({_numel});

            if (is_contiguous()) {
               for (size_t i = 0; i < _numel; ++i) {
                  result._storage->at(i) = predicate(this->_storage->at(this->_offset + i));
               }
               return result.reshape(_shape);
            }

            auto it = Iterator(_shape, _strides, _storage, _offset);
            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = predicate(it.get());
               it.next();
            }

            return result.reshape(_shape);
         }

         T trace() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("Trace requires a square 2D tensor.");
            }

            T total = T(0);
            size_t n = _shape[0];
            size_t row_stride = _strides[0];
            size_t col_stride = _strides[1];

            for (size_t i = 0; i < n; ++i) {
               total += _storage->at(_offset + (i * row_stride) + (i * col_stride));
            }

            return total;
         }

         // Retorna {Matriz_LU, Vetor_de_Permutacao, Numero_de_Trocas}
         std::tuple<Tensor<T>, std::vector<size_t>, int> lu_decomposition() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("LU decomposition requires a square 2D tensor.");
            }

            size_t n = _shape[0];
            auto LU = this->clone(); 
            auto lu_at = [&](size_t r, size_t c) -> T& { return LU._storage->at(r * n + c); };

            // Vetor para rastrear as permutações (essencial para a Inversa)
            std::vector<size_t> P(n);
            for (size_t i = 0; i < n; ++i) {
               P[i] = i;
            }
            
            int swaps = 0;

            for (size_t j = 0; j < n; ++j) {
               // 1. Atualiza a coluna j para L e U ANTES do pivotamento
               for (size_t i = 0; i < n; ++i) {
                  size_t limit = std::min(i, j);
                  T sum = T(0);
                  for (size_t k = 0; k < limit; ++k) {
                     sum += lu_at(i, k) * lu_at(k, j);
                  }
                  lu_at(i, j) -= sum;
               }

               // 2. Pivotamento Parcial: procura da diagonal (j) para baixo
               size_t max_row = j;
               T max_value = std::abs(lu_at(j, j));
               for (size_t i = j + 1; i < n; ++i) {
                  if (std::abs(lu_at(i, j)) > max_value) {
                     max_value = std::abs(lu_at(i, j));
                     max_row = i;
                  }
               }

               if (max_value == T(0)) {
                  throw std::domain_error("Matrix is singular (zero pivot encountered).");
               }

               // 3. Troca as linhas na matriz e atualiza o rastreador P
               if (max_row != j) {
                  for (size_t k = 0; k < n; ++k) {
                     std::swap(lu_at(j, k), lu_at(max_row, k));
                  }
                  std::swap(P[j], P[max_row]);
                  swaps++;
               }

               // 4. Calcula os multiplicadores de L (divide pelo pivô)
               for (size_t i = j + 1; i < n; ++i) {
                  lu_at(i, j) /= lu_at(j, j);
               }
            }

            return {LU, P, swaps};
         }

         T determinant() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("Determinant requires a square 2D tensor.");
            }
         
            auto [LU, P, swaps] = lu_decomposition();
            
            // Inverte o sinal dependendo se a quantidade de trocas foi par ou ímpar
            T det = (swaps % 2 == 0) ? T(1) : T(-1);

            for (size_t i = 0; i < _shape[0]; ++i) {
               det *= LU._storage->at(i * _shape[0] + i);
            }

            return det;
         }

         Tensor<T> inverse() const {
            if (_shape.size() != 2 || _shape[0] != _shape[1]) {
               throw std::invalid_argument("Inverse requires a square 2D tensor.");
            }

            size_t n = _shape[0];
            
            // 1. Fazemos a decomposição LU. 
            // Não precisamos verificar o determinante aqui, pois se a matriz 
            // for singular, lu_decomposition() já lançará uma exceção no pivô nulo.
            auto [LU, P, swaps] = lu_decomposition();
            
            // Inicia a matriz resultante com zeros
            Tensor<T> result = Tensor<T>::zeros({n, n});
            
            T* lu_data = LU._storage->data();
            T* res_data = result._storage->data();

            auto lu_at = [&](size_t r, size_t c) -> const T& { return lu_data[r * n + c]; };
            auto res_at = [&](size_t r, size_t c) -> T& { return res_data[r * n + c]; };

            // 2. Monta a matriz Identidade Permutada (P * I) no resultado.
            // O vetor P guardou o índice original de cada linha.
            for (size_t i = 0; i < n; ++i) {
               res_at(i, P[i]) = T(1);
            }

            // 3. Forward Substitution: Resolve L * Y = P * I
            // Como L tem 1s implícitos na diagonal, a divisão por L(i,i) não é necessária.
            for (size_t col = 0; col < n; ++col) {
               for (size_t i = 0; i < n; ++i) {
                  for (size_t k = 0; k < i; ++k) {
                     res_at(i, col) -= lu_at(i, k) * res_at(k, col);
                  }
               }
            }

            // 4. Backward Substitution: Resolve U * X = Y
            // U contém elementos não unitários na diagonal principal, então precisamos dividir.
            for (size_t col = 0; col < n; ++col) {
               // Usamos (int) para i não dar underflow, já que size_t é unsigned
               for (int i = (int)n - 1; i >= 0; --i) {
                  for (size_t k = i + 1; k < n; ++k) {
                     res_at(i, col) -= lu_at(i, k) * res_at(k, col);
                  }
                  res_at(i, col) /= lu_at(i, i);
               }
            }

            return result;
         }

         bool equals_to(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               return false;
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);

            for (size_t i = 0; i < _numel; ++i) {
               if (lhsIt.get() != rhsIt.get()) {
                  return false;
               }
               lhsIt.next();
               rhsIt.next();
            }

            return true;
         }

         Tensor<bool> operator==(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhsIt.get() == rhsIt.get());
               lhsIt.next();
               rhsIt.next();
            }

            return result;
         }

         Tensor<bool> operator!=(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhsIt.get() != rhsIt.get());
               lhsIt.next();
               rhsIt.next();
            }

            return result;
         }

         Tensor<bool> operator<(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhsIt.get() < rhsIt.get());
               lhsIt.next();
               rhsIt.next();
            }

            return result;
         }

         Tensor<bool> operator>(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhsIt.get() > rhsIt.get());
               lhsIt.next();
               rhsIt.next();
            }

            return result;
         }

         Tensor<bool> operator>=(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhsIt.get() >= rhsIt.get());
               lhsIt.next();
               rhsIt.next();
            }

            return result;
         }

         Tensor<bool> operator<=(const Tensor<T>& other) const {
            if (_shape != other._shape) {
               throw std::invalid_argument("Tensor shapes must match");
            }

            auto lhsIt = Iterator(_shape, _strides, _storage, _offset);
            auto rhsIt = Iterator(other._shape, other._strides, other._storage, other._offset);
            Tensor<bool> result(_shape);

            for (size_t i = 0; i < _numel; ++i) {
               result._storage->at(i) = (lhsIt.get() <= rhsIt.get());
               lhsIt.next();
               rhsIt.next();
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
