#pragma once
#include <cassert>
#include <utility>

namespace foundation {

template <class T>
class Rc final {
 public:
  class Object {
   private:
    Object(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) = delete;

   private:
    int count_{1};

   protected:
    Object() = default;
    virtual ~Object() = default;

   public:
    void grab() {
      assert(count_ > 0);
      ++count_;
    }
    void drop() {
      assert(count_ > 0);
      if (--count_ > 0) return;
      on_drop();
    }
    int reference_count() const { return count_; }

   protected:
    virtual void on_drop() {}
  };

 private:
  T* pointer_{nullptr};

 public:
  Rc() = default;
  Rc(T* p, bool with_grab) : pointer_{p} {
    if (pointer_ && with_grab) {
      pointer_->grab();
    }
  }
  Rc(const Rc& rhs) : pointer_{rhs.pointer_} {
    if (pointer_) {
      pointer_->grab();
    }
  }
  Rc(Rc&& rhs) : pointer_{rhs.pointer_} { rhs.pointer_ = nullptr; }
  ~Rc() {
    if (pointer_) {
      pointer_->drop();
    }
  }

  T* get() const { return pointer_; }
  T* operator->() const { return pointer_; }
  T& operator*() const { return *pointer_; }
  explicit operator bool() const { return !!pointer_; }

  void swap(Rc& rhs) {
    using std::swap;
    swap(pointer_, rhs.pointer_);
  }

  Rc& operator=(const Rc& rhs) {
    Rc(rhs).swap(*this);
    return *this;
  }
  Rc& operator=(Rc&& rhs) {
    Rc(std::move(rhs)).swap(*this);
    return *this;
  }

  bool operator==(const Rc& rhs) const { return pointer_ == rhs.pointer_; }
  bool operator!=(const Rc& rhs) const { return pointer_ != rhs.pointer_; }
};

template <class T>
inline void swap(Rc<T>& lhs, Rc<T>& rhs) {
  lhs.swap(rhs);
}

}  // namespace foundation
