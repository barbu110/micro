// Copyright 2019 Stefan Silviu

#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace microloop
{

class Buffer
{
  std::size_t sz;
  void *buf;

public:
  /**
   * Create a not-allocated buffer.
   */
  Buffer() noexcept : sz{0}, buf{nullptr}
  {}

  /**
   * Create a zeroed buffer of given size.
   * @param sz The size of the buffer.
   */
  explicit Buffer(std::size_t sz) noexcept : sz{sz}, buf{calloc(sz, sizeof(std::uint8_t))}
  {}

  /**
   * Create a buffer by copying `sz` bytes from the given `src` char array.
   * @param src The source char array to copy from.
   * @param sz How many bytes to copy from the source array.
   */
  Buffer(const char *src, std::size_t sz) : Buffer{sz}
  {
    std::memcpy(buf, src, sz);
  }

  /**
   * Create a buffer from the given string. The size of the buffer will be the size of the given
   * string including the null character.
   * @param src The source string.
   */
  Buffer(const char *src) : Buffer{strlen(src) + 1}
  {
    std::memcpy(buf, src, sz);
  }

  ~Buffer()
  {
    free(buf);
  }

  /**
   * Copy constructor.
   */
  Buffer(const Buffer &other) noexcept : Buffer{other.sz}
  {
    std::memcpy(buf, other.buf, other.sz);
  }

  /**
   * Move constructor.
   */
  Buffer(Buffer &&) noexcept = default;

  /**
   * Copy/move assignment.
   */
  Buffer &operator=(Buffer other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  /**
   * Retrieve the raw data held within the buffer
   */
  void *data() const noexcept
  {
    return buf;
  }

  /**
   * Get the buffer data as an std::string. Note that this will work well for buffers that are
   * suitable to be read as strings, but not for binary information, such as structures.
   */
  std::string str() const
  {
    const std::string::value_type *s = reinterpret_cast<std::string::value_type *>(buf);
    return std::string{s, size()};
  }

  /**
   * Retrieve the size of the buffer.
   */
  std::size_t size() const noexcept
  {
    return sz;
  }

  /**
   * @return Whether the buffer is empty or not.
   */
  bool empty() const noexcept
  {
    return sz == 0;
  }

  /**
   * Resize the buffer to the given size. Note that in case of enlarging the size of the buffer,
   * there is no guarantee that the new memory area will be zeroed out.
   */
  void resize(std::size_t new_size) noexcept
  {
    buf = realloc(buf, new_size);
    sz = new_size;
  }

  friend void swap(Buffer &lhs, Buffer &rhs) noexcept
  {
    using std::swap;
    swap(lhs.sz, rhs.sz);
    swap(lhs.buf, rhs.buf);
  }
};

}  // namespace microloop
