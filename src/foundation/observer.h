#pragma once
#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace foundation {

template <class T>
class Observer;

template <class T>
class Subject {
 public:
  template <class T>
  struct Event {
    size_t index;
    T data;

    template <class... Args>
    Event(size_t index, Args&&... data)
        : index{index}, data{std::forward<Args>(data)...} {}
  };

 private:
  std::array<std::vector<Event<T>>, 2> events_;
  std::array<size_t, 2> start_index_{0, 0};
  size_t count_{0};
  size_t index_{0};

 public:
  Observer<T> make_observer() { return Observer<T>{this, 0}; }
  Observer<T> make_observer_current() { return Observer<T>{this, count_}; }

  void update() {
    const auto index = 1 - index_;
    events_[index].clear();
    start_index_[index] = count_;
    index_ = index;
  }

  template <class... Args>
  void notify(Args&&... args) {
    events_[index_].emplace_back(count_++, std::forward<Args>(args)...);
  }
  void clear_events() {
    for (size_t i = 0; i < 2; ++i) {
      events_[i].clear();
      start_index[i] = count_;
    }
  }

  const Event<T>* get_event(size_t index) const {
    const auto n = count_ - index;
    if (n == 0) return nullptr;
    if (n <= events_[index_].size()) {
      const auto i = index - start_index_[index_];
      return &events_[index_][i];
    } else {
      auto last_index = 1 - index_;
      auto i = index - start_index_[last_index];
      const auto& data = events_[last_index];
      if (i < data.size()) {
        return &data[i];
      } else if (!data.empty()) {
        return &data[0];
      }
    }
    return nullptr;
  }
};

template <class T>
class Observer {
 private:
  const Subject<T>* subject_ = nullptr;
  size_t index_ = 0;

 public:
  Observer() = default;
  Observer(const Subject<T>* subject, size_t index)
      : subject_{subject}, index_{index} {}

 public:
  const T* next() {
    if (!subject_) return nullptr;
    if (auto e = subject_->get_event(index_)) {
      index_ = e->index + 1;
      return &e->data;
    }
    return nullptr;
  }
};

}  // namespace foundation
