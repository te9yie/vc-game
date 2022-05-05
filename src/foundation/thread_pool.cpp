#include "thread_pool.h"

namespace foundation {

/*explicit*/ ThreadPool::ThreadPool(size_t n) : threads_{n} {}
/*virtual*/ ThreadPool::~ThreadPool() {
  for (size_t i = 0; i < next_index_; ++i) {
    SDL_WaitThread(threads_[i], nullptr);
  }
}

bool ThreadPool::create(std::string_view name, Func func, void* arg) {
  if (next_index_ >= threads_.size()) return false;

  auto thread = SDL_CreateThread(func, name.data(), arg);
  if (!thread) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
    return false;
  }

  threads_[next_index_++] = thread;
  return true;
}

}  // namespace foundation
