#ifndef PROGRESS_BAR_HPP
#define PROGRESS_BAR_HPP

#include <stddef.h>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

class ProgressBar {
   private:
      class GenericType {
         public:
            virtual ~GenericType() = default;
            virtual std::string renderLine() const = 0;
      };

      template <typename T>
      class ProgressAtribute : public GenericType {
         public:
            std::string title;
            T value;
            std::string unit;

            ProgressAtribute(const std::string& title, const T& value = {}): 
               title(title), value(value) {}
            ProgressAtribute(const std::string& title, const std::string& unit, const T& value = {}): 
               title(title), value(value), unit(unit) {}

            std::string renderLine() const override {
               std::ostringstream output;
               output << title << ": " << value;

               if (!unit.empty()) {
                  output << " " << unit;
               }

               return output.str();
            }
      };

      class ProgressAxis {
         public:
            uint init, end;
            std::string title;

            ProgressAxis(uint end):
               init(0), end(end) {}
            ProgressAxis(uint init, uint end):
               init(init), end(end) {}
            ProgressAxis(uint init, uint end, const std::string& title):
               init(init), end(end), title(title) {}
            ProgressAxis(uint end, const std::string& title):
               init(0), end(end), title(title) {}
            ProgressAxis(std::tuple<uint, uint> range):
               init(std::get<0>(range)), end(std::get<1>(range)) {}
            ProgressAxis(std::tuple<uint, uint, std::string> data):
               init(std::get<0>(data)), end(std::get<1>(data)), title(std::get<2>(data)) {}
            ProgressAxis(std::tuple<uint, std::string> data):
               init(0), end(std::get<0>(data)), title(std::get<1>(data)) {}
      };

      class Iterator {
         public:
            class AxisState {
               private:
                  const std::vector<uint>* _indices = nullptr;

               public:
                  AxisState() = default;
                  explicit AxisState(const std::vector<uint>* indices):
                     _indices(indices) {}

                  uint axis(size_t index) const {
                     if (_indices == nullptr) {
                        throw std::runtime_error("Axis state is not initialized");
                     }

                     if (index >= _indices->size()) {
                        throw std::out_of_range("Axis index out of range");
                     }

                     return (*_indices)[index];
                  }

                  uint operator[](size_t index) const {
                     return axis(index);
                  }

                  size_t size() const {
                     return _indices == nullptr ? 0 : _indices->size();
                  }

                  const std::vector<uint>& values() const {
                     if (_indices == nullptr) {
                        throw std::runtime_error("Axis state is not initialized");
                     }

                     return *_indices;
                  }

                  std::vector<uint>::const_iterator begin() const {
                     return values().begin();
                  }

                  std::vector<uint>::const_iterator end() const {
                     return values().end();
                  }
            };

         private:
            std::vector<ProgressAxis> _axis;
            std::vector<uint> _current_indices;
            bool _is_end = false;
            bool _forward = true;
            ProgressBar* _owner = nullptr;
            bool _render_on_step = false;
            size_t _step = 0;
            mutable AxisState _axis_state;

            void incrementIndices();
            void decrementIndices();

         public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = AxisState;
            using difference_type = std::ptrdiff_t;
            using pointer = const value_type*;
            using reference = value_type;

            Iterator() = default;
            Iterator(const std::vector<ProgressAxis>& axis,
                     bool forward = true,
                     ProgressBar* owner = nullptr,
                     bool render_on_step = false,
                     bool is_end = false);

            reference operator*() const;
            pointer operator->() const;

            Iterator& operator++();
            Iterator operator++(int);
            Iterator& operator--();
            Iterator operator--(int);
            bool operator==(const Iterator& rhs) const;
            bool operator!=(const Iterator& rhs) const;

            const std::vector<uint>& indices() const;
            size_t step() const;
            void detachOwner();
            bool isEnd() const;
      };

      using AttrPtr = std::shared_ptr<GenericType>;
      using AxisRange = std::tuple<uint, uint>;
      using AxisDataU = std::tuple<uint, std::string>;
      using AxisData = std::tuple<uint, uint, std::string>;
      static const uint8_t MIN_BAR_WIDTH = 1;
      static const uint8_t FREE_SPACE_IN_BAR_ROW = 10;

      std::vector<ProgressAxis> _axis;
      std::string _title;
      uint8_t _bar_width;
      std::unordered_map<std::string, AttrPtr> _attributes;
      Iterator _progress_iterator;
      int _current_progress = 0;
      int _total_progress = 0;
      bool _verbose = false;
      mutable size_t _last_rendered_lines = 0;

      uint8_t getTerminalWidth() const;
      void clearPreviousRender() const;
      void syncIteratorState(const Iterator& iterator);

      template <typename AxisType>
      void constructor(std::initializer_list<AxisType> axis) {
         static_assert(
            std::is_same_v<AxisType, AxisData> ||
            std::is_same_v<AxisType, AxisDataU> ||
            std::is_same_v<AxisType, AxisRange>,
            "Invalid axis type for ProgressBar constructor"
         );

         _total_progress = 1;
         int axis_index = 0;

         if (axis.size() == 0) {
            throw std::invalid_argument("At least one axis must be provided");
         }

         std::vector<ProgressAxis> axis_vector;

         for (const auto& tuple : axis) {
            uint range_init, range_end;

            if constexpr (std::is_same_v<AxisType, AxisDataU>) {
               range_init = 0;
               range_end = std::get<0>(tuple);
            }
            else {
               range_init = std::get<0>(tuple);
               range_end = std::get<1>(tuple);
            }

            if (range_end <= range_init) {
               throw std::invalid_argument("Axis " + std::to_string(axis_index) + " end must be greater than init");
            }

            auto item = ProgressAxis(tuple);
            axis_vector.emplace_back(item);

            _total_progress *= (range_end - range_init);
            axis_index++;
         }

         _axis = axis_vector;
         _title = "";
         auto terminal_width = getTerminalWidth();
         _bar_width = terminal_width > FREE_SPACE_IN_BAR_ROW ?
            terminal_width - FREE_SPACE_IN_BAR_ROW :
            MIN_BAR_WIDTH;
         _progress_iterator = Iterator(_axis);
         _current_progress = 0;
         _last_rendered_lines = 0;
      }

   public:
      ProgressBar(uint end);
      ProgressBar(uint init, uint end);
      ProgressBar(uint init, uint end, const std::string& axis_title);
      ProgressBar(std::initializer_list<AxisRange> ranges);
      ProgressBar(std::initializer_list<AxisDataU> data);
      ProgressBar(std::initializer_list<AxisData> data);
   
      template <typename T>
      ProgressBar& createAttribute(const std::string& title, const T& value = {}) {
         auto ptr = std::make_shared<ProgressAtribute<T>>(title, value);
         _attributes.emplace(title, ptr);
         return *this;
      }

      template <typename T>
      ProgressBar& createAttribute(const std::string& title, const std::string& unit, const T& value = {}) {
         auto ptr = std::make_shared<ProgressAtribute<T>>(title, unit, value);
         _attributes.emplace(title, ptr);
         return *this;
      }

      template <typename T>
      ProgressBar& updateAttribute(const std::string& title, const T& value) {
         auto it = _attributes.find(title);
         if (it != _attributes.end()) {
            using ValueType = std::remove_cvref_t<T>;
            auto ptr = std::dynamic_pointer_cast<ProgressAtribute<ValueType>>(it->second);
            if (!ptr) {
               throw std::runtime_error("Attribute type mismatch for title: " + title);
            }

            ptr->value = value;
         } else {
            throw std::runtime_error("Attribute not found for title: " + title);
         }
         return *this;
      }

      ProgressBar& setTitle(const std::string& title);
      std::string getTitle() const;

      ProgressBar& setVerbose(bool verbose);
      bool isVerbose() const;

      ProgressBar& setBarWidth(uint8_t width);
      uint8_t getBarWidth() const;

      void render() const;
      void step();
      bool isComplete() const;
      
      Iterator begin();
      Iterator end();
      Iterator rbegin();
      Iterator rend();
};

#endif // !PROGRESS_BAR_HPP