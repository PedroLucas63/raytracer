#ifndef TENSOR_STORAGE_HPP
#define TENSOR_STORAGE_HPP

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace raytracer
{
   template <typename T>
   class Storage
   {
      public:
         virtual ~Storage() = default;

         virtual T* data() = 0;
         virtual const T* data() const = 0;

         virtual T& at(size_t idx) = 0;
         virtual const T& at(size_t idx) const = 0;

         virtual size_t size() const = 0;
   };

   template <typename T>
   class CpuStorage : public Storage<T>
   {
      private:
         std::unique_ptr<T[]> _data;
         size_t _size;

      public:
         explicit CpuStorage(size_t size)
            : _data(std::make_unique<T[]>(size)),
              _size(size)
         {}

         ~CpuStorage() override = default;

         T* data() override {
            return _data.get();
         }

         const T* data() const override {
            return _data.get();
         }

         T& at(size_t idx) override {
            return _data[idx];
         }

         const T& at(size_t idx) const override {
            return _data[idx];
         }

         size_t size() const override {
            return _size;
         }
   };
}

#endif // TENSOR_STORAGE_HPP
