#pragma once
#include <SDL.h>

#include <cstddef>
#include <string_view>
#include <vector>

namespace foundation {

class ThreadPool {
 private:
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

 public:
  using Func = int (*)(void*);

 private:
  std::vector<SDL_Thread*> threads_;
  size_t next_index_ = 0;

 public:
  explicit ThreadPool(size_t n);
  virtual ~ThreadPool();

  bool create(std::string_view name, Func func, void* arg);

  size_t remain_count() const { return threads_.size() - next_index_; }
};

}  // namespace foundation
