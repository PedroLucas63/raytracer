#include "ProgressBar.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

ProgressBar::Iterator::Iterator(const std::vector<ProgressAxis>& axis,
                                bool forward,
                                ProgressBar* owner,
                                bool render_on_step,
                                bool is_end)
   : _axis(axis),
     _is_end(is_end),
     _forward(forward),
     _owner(owner),
     _render_on_step(render_on_step) {
   _current_indices.reserve(_axis.size());

   for (const auto& item : _axis) {
      _current_indices.emplace_back(_forward ? item.init : item.end - 1);
   }

   if (_is_end && _owner != nullptr) {
      _step = static_cast<size_t>(_owner->_total_progress);
   }
}

ProgressBar::Iterator::reference ProgressBar::Iterator::operator*() const {
   _axis_state = AxisState(&_current_indices);
   return _axis_state;
}

ProgressBar::Iterator::pointer ProgressBar::Iterator::operator->() const {
   _axis_state = AxisState(&_current_indices);
   return &_axis_state;
}

void ProgressBar::Iterator::incrementIndices() {
   auto next_indices = _current_indices;

   for (size_t i = 0; i < _axis.size(); ++i) {
      if (_forward) {
         if (next_indices[i] < _axis[i].end - 1) {
            next_indices[i]++;
            for (size_t j = 0; j < i; ++j) {
               next_indices[j] = _axis[j].init;
            }

            _current_indices = next_indices;
            return;
         }
      } else {
         if (next_indices[i] > _axis[i].init) {
            next_indices[i]--;
            for (size_t j = 0; j < i; ++j) {
               next_indices[j] = _axis[j].end - 1;
            }

            _current_indices = next_indices;
            return;
         }
      }
   }

   // Reached past-the-end: keep the last valid indices for final 100% render.
   _is_end = true;
}

void ProgressBar::Iterator::decrementIndices() {
   for (size_t offset = 0; offset < _axis.size(); ++offset) {
      const auto i = _axis.size() - offset - 1;

      if (_forward) {
         if (_current_indices[i] > _axis[i].init) {
            _current_indices[i]--;
            return;
         }

         _current_indices[i] = _axis[i].end - 1;
      } else {
         if (_current_indices[i] < _axis[i].end - 1) {
            _current_indices[i]++;
            return;
         }

         _current_indices[i] = _axis[i].init;
      }
   }

   _is_end = true;
}

ProgressBar::Iterator& ProgressBar::Iterator::operator++() {
   if (_is_end) {
      return *this;
   }

   incrementIndices();
   ++_step;

   if (_owner != nullptr) {
      _owner->syncIteratorState(*this);
      if (_render_on_step) {
         _owner->render();
      }
   }

   return *this;
}

ProgressBar::Iterator ProgressBar::Iterator::operator++(int) {
   Iterator temp = *this;
   ++(*this);
   return temp;
}
ProgressBar::Iterator& ProgressBar::Iterator::operator--() {
   if (_is_end) {
      _is_end = false;

      if (_step > 0) {
         _step--;
      }

      if (_owner != nullptr) {
         _owner->syncIteratorState(*this);
         if (_render_on_step) {
            _owner->render();
         }
      }

      return *this;
   }

   decrementIndices();

   if (_step > 0) {
      _step--;
   }

   if (_owner != nullptr) {
      _owner->syncIteratorState(*this);
      if (_render_on_step && !_is_end) {
         _owner->render();
      }
   }

   return *this;
}

ProgressBar::Iterator ProgressBar::Iterator::operator--(int) {
   Iterator temp = *this;
   --(*this);
   return temp;
}

bool ProgressBar::Iterator::operator==(const Iterator& rhs) const {
   if (_is_end && rhs._is_end) {
      return true;
   }

   if (_is_end != rhs._is_end) {
      return false;
   }

   if (_axis.size() != rhs._axis.size()) {
      return false;
   }

   if (_current_indices != rhs._current_indices) {
      return false;
   }

   return true;
}

bool ProgressBar::Iterator::operator!=(const Iterator& rhs) const {
   return !(*this == rhs);
}

const std::vector<unsigned int>& ProgressBar::Iterator::indices() const {
   return _current_indices;
}

size_t ProgressBar::Iterator::step() const {
   return _step;
}

void ProgressBar::Iterator::detachOwner() {
   _owner = nullptr;
   _render_on_step = false;
}

bool ProgressBar::Iterator::isEnd() const {
   return _is_end;
}

ProgressBar::ProgressBar(uint end) {
   constructor({AxisRange(0, end)});
}

ProgressBar::ProgressBar(uint init, uint end) {
   constructor({AxisRange(init, end)});
}

ProgressBar::ProgressBar(uint init, uint end, const std::string& title) {
   constructor({AxisData(init, end, title)});
}

ProgressBar::ProgressBar(std::initializer_list<AxisRange> ranges) {
   constructor(ranges);
}

ProgressBar::ProgressBar(std::initializer_list<AxisDataU> data) {
   constructor(data);
}

ProgressBar::ProgressBar(std::initializer_list<AxisData> data) {
   constructor(data);
}

ProgressBar& ProgressBar::setTitle(const std::string& title) {
   _title = title;
   return *this;
}

std::string ProgressBar::getTitle() const {
   return _title;
}

ProgressBar& ProgressBar::setBarWidth(uint8_t width) {
   if (MIN_BAR_WIDTH > width) {
      width = MIN_BAR_WIDTH;
   }

   _bar_width = width;
   return *this;
}

uint8_t ProgressBar::getBarWidth() const {
   return _bar_width;
}

uint8_t ProgressBar::getTerminalWidth() const {
   uint8_t cols = 80;

   #if defined(_WIN32)
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
         cols = static_cast<uint8_t>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
      }
   #else
      struct winsize w {};
      if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
         cols = static_cast<uint8_t>(w.ws_col);
      }
   #endif

   return cols;
}

void ProgressBar::clearPreviousRender() const {
   #if !defined(_WIN32)
      if (!isatty(STDOUT_FILENO)) {
         return;
      }
   #endif

   if (_last_rendered_lines == 0) {
      return;
   }

   std::cout << "\033[" << _last_rendered_lines << "A";

   for (size_t i = 0; i < _last_rendered_lines; ++i) {
      std::cout << "\r\033[2K";
      if (i + 1 < _last_rendered_lines) {
         std::cout << "\033[1B";
      }
   }

   if (_last_rendered_lines > 1) {
      std::cout << "\033[" << (_last_rendered_lines - 1) << "A";
   }

   std::cout << "\r";
}

void ProgressBar::syncIteratorState(const Iterator& iterator) {
   _progress_iterator = iterator;
   _progress_iterator.detachOwner();

   _current_progress = static_cast<int>(iterator.step());
   if (_current_progress > _total_progress) {
      _current_progress = _total_progress;
   }
}

void ProgressBar::render() const {
   std::vector<std::string> lines;

   if (_verbose) {
      lines.emplace_back(_title.empty() ? "Progress" : _title);
   }

   float progress_percentage = 0.0f;
   if (_total_progress > 0) {
      progress_percentage = (100.0f * static_cast<float>(_current_progress)) / static_cast<float>(_total_progress);
   }

   progress_percentage = std::clamp(progress_percentage, 0.0f, 100.0f);

   uint8_t filled_length = static_cast<uint8_t>((progress_percentage / 100.0f) * static_cast<float>(_bar_width));
   if (filled_length > _bar_width) {
      filled_length = _bar_width;
   }

   auto repeatUtf8 = [](const std::string& glyph, size_t count) {
      std::string output;
      output.reserve(glyph.size() * count);

      for (size_t i = 0; i < count; ++i) {
         output += glyph;
      }

      return output;
   };

   const auto filled_bar = repeatUtf8("█", static_cast<size_t>(filled_length));
   const auto empty_bar = repeatUtf8("░", static_cast<size_t>(_bar_width - filled_length));

   std::ostringstream bar_line;
   bar_line << "▏" << filled_bar << empty_bar << "▕ "
            << std::fixed << std::setprecision(2) << progress_percentage << "%";
   lines.emplace_back(bar_line.str());

   lines.emplace_back("");

   if (_verbose) {
      const auto& indices = _progress_iterator.indices();
      for (size_t i = 0; i < _axis.size(); ++i) {
         const auto& axis = _axis[i];
         const auto axis_range = axis.end - axis.init;
         const auto current_value = (i < indices.size()) ? indices[i] : axis.init;
         const auto axis_step = (current_value - axis.init) + 1;

         std::ostringstream axis_line;
         if (!axis.title.empty()) {
            axis_line << axis.title;
         } else {
            axis_line << "Axis " << i;
         }

         axis_line << ": " << axis_step << "/" << axis_range;
         lines.emplace_back(axis_line.str());
      }

      lines.emplace_back("");
      lines.emplace_back("");

      if (_attributes.empty()) {
         lines.emplace_back("Attributes: (none)");
      } else {
         for (const auto& [name, attribute] : _attributes) {
            (void)name;
            lines.emplace_back(attribute->renderLine());
         }
      }
   }

   clearPreviousRender();

   for (const auto& line : lines) {
      std::cout << line << '\n';
   }

   std::cout.flush();
   _last_rendered_lines = lines.size();
}

void ProgressBar::step() {
   if (_current_progress >= _total_progress) {
      throw std::runtime_error("Progress is already complete");
   }

   ++_progress_iterator;
   _current_progress = static_cast<int>(_progress_iterator.step());
}

bool ProgressBar::isComplete() const {
   return _current_progress >= _total_progress;
}

ProgressBar::Iterator ProgressBar::begin() {
   Iterator iterator(_axis, true, this, true);
   syncIteratorState(iterator);
   render();
   return iterator;
}

ProgressBar::Iterator ProgressBar::end() {
   return Iterator(_axis, true, this, true, true);
}

ProgressBar::Iterator ProgressBar::rbegin() {
   Iterator iterator(_axis, false, this, true);
   syncIteratorState(iterator);
   render();
   return iterator;
}

ProgressBar::Iterator ProgressBar::rend() {
   return Iterator(_axis, false, this, true, true);
}

ProgressBar& ProgressBar::setVerbose(bool verbose) {
   _verbose = verbose;
   return *this;
}

bool ProgressBar::isVerbose() const {
   return _verbose;
}