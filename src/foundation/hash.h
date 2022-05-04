#pragma once
#include <cstdint>
#include <cstring>

namespace foundation {

constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;
constexpr uint32_t FNV_PRIME = 16777619u;

inline void fnv_1a(uint32_t* hash, const void* buf, size_t n) {
  auto p = reinterpret_cast<const uint8_t*>(buf);
  while (n--) {
    *hash = (*hash ^ *p++) * FNV_PRIME;
  }
}

inline uint32_t make_string_hash(const char* str) {
  uint32_t hash = FNV_OFFSET_BASIS;
  fnv_1a(&hash, str, std::strlen(str));
  return hash;
}

}  // namespace foundation
