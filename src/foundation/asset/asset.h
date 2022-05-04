#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "foundation/reference_count.h"

namespace foundation {
namespace asset {

class Asset : public Rc<Asset>::Object {
 public:
  enum class State {
    NotLoaded,
    Loading,
    Loaded,
  };

 private:
  const std::string file_path_;
  const uint32_t path_hash_ = 0;
  std::vector<uint8_t> data_;
  State state_ = State::NotLoaded;

 public:
  explicit Asset(const std::string_view& path);

  bool load();

  const std::string& file_path() const { return file_path_; }
  uint32_t path_hash() const { return path_hash_; }
  const std::vector<uint8_t>& data() const { return data_; }

 private:
  void loaded();

 protected:
  virtual void on_loaded() {}

 protected:
  // Rc<Asset>::Object.
  virtual void on_drop() override;
};

}  // namespace asset
}  // namespace foundation
