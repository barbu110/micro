// Copyright 2019 Stefan Silviu

#pragma once

#include <cstring>
#include <string>

namespace microloop {

class Buffer {
  std::size_t size;
  void *buf;

public:
  explicit Buffer(std::size_t size) noexcept : size(size), buf(malloc(size))
  {}

  Buffer(const char *src, std::size_t size) : Buffer(size)
  {
    std::memcpy(buf, src, size);
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
  Buffer(const std::string &src) : Buffer(src.c_str(), src.length())
  {}
#pragma clang diagnostic pop

  ~Buffer()
  {
    free(buf);
  }

  Buffer(const Buffer &other) noexcept : Buffer(other.size)
  {
    std::memcpy(buf, other.buf, other.size);
  }

  Buffer &operator=(Buffer other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  Buffer(Buffer &&) noexcept = default;
  Buffer &operator=(Buffer &&) noexcept = default;

  void *data() const noexcept
  {
    return buf;
  }

  std::size_t getSize() const noexcept
  {
    return size;
  }

  void resize(std::size_t new_size) noexcept
  {
    buf = realloc(buf, new_size);
    size = new_size;
  }

  friend void swap(Buffer &lhs, Buffer &rhs) noexcept
  {
    using std::swap;
    swap(lhs.size, rhs.size);
    swap(lhs.buf, rhs.buf);
  }
};
}  // namespace microloop
