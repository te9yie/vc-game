#include "asset.h"

#include <SDL.h>

#include <memory>

#include "foundation/hash.h"

namespace foundation {
namespace asset {

namespace {

struct CloseOps {
  void operator()(SDL_RWops* ops) const { SDL_RWclose(ops); }
};
using RWopsPtr = std::unique_ptr<SDL_RWops, CloseOps>;

}  // namespace

/*explicit*/ Asset::Asset(const std::string_view& path)
    : file_path_{path.begin(), path.end()},
      path_hash_{make_string_hash(path.data())} {}

bool Asset::load() {
  SDL_assert(data_.empty());

  RWopsPtr file(SDL_RWFromFile(file_path().c_str(), "rb"));
  if (!file) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", file_path().c_str(),
                 SDL_GetError());
    return false;
  }
  auto size = SDL_RWsize(file.get());
  if (size <= 0) {
    loaded();
    return true;
  }

  state_ = State::Loading;

  data_.resize(size);

  Sint64 total = 0;
  Sint64 read = 0;
  auto p = data_.data();
  do {
    read = SDL_RWread(file.get(), p, 1, size - total);
    total += read;
    p += read;
  } while (total < size && read != 0);

  loaded();

  return true;
}

void Asset::loaded() {
  on_loaded();
  state_ = State::Loaded;
}

/*virtual*/ void Asset::on_drop() /*override*/
{}

}  // namespace asset
}  // namespace foundation
