// Copyright 2019 Stefan Silviu

#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <algorithm>

namespace microloop
{

class Buffer
{
  std::size_t sz;
  void *buf;

public:
  /**
   * \brief Create a not-allocated buffer.
   *
   * \param sz The size of the buffer.
   */
  Buffer(std::size_t sz = 0) noexcept : sz{sz}, buf{sz ? calloc(sz, sizeof(std::uint8_t)) : nullptr}
  {}

  /**
   * \brief Create a buffer by copying \p sz bytes from the given \p src char array.
   *
   * \param src The source char array to copy from.
   * \param sz How many bytes to copy from the source array.
   */
  Buffer(const char *src, std::size_t sz) : Buffer{sz}
  {
    if (sz)
    {
      std::memcpy(buf, src, sz);
    }
  }

  /**
   * \brief Create a buffer from the given string. The size of the buffer will be the size of the
   * given string without the null character.
   * \param src The source string.
   */
  Buffer(const char *src) : Buffer{strlen(src)}
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
   * \brief Concatenate \p count bytes of the \p other buffer to this one.
   *
   * \param other The buffer from which the bytes are to be extracted.
   * \param count How many bytes are to be extracted from the other buffer and concatenated to this
   * one.
   */
  void concat(const Buffer &other, std::size_t count) noexcept
  {
    if (!count)
    {
      return;
    }

    auto curr_size = size();
    resize(size() + count);
    std::memcpy(buf + curr_size, other.data(), count);
  }

  /**
   * \brief Concatenate the entire \p other buffer to this one.
   *
   * \param other The buffer to be concatenated to this one.
   */
  void concat(const Buffer &other) noexcept
  {
    concat(other, other.size());
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
   * \brief Get an std::string_view from this buffer.
   * \param start_idx Offset to start at, in bytes
   * \param n_bytes How many bytes to be included
   */
  std::string_view str_view(std::size_t start_idx, std::size_t n_bytes) const
  {
    const std::string_view::value_type *s = reinterpret_cast<std::string_view::value_type *>(buf);
    return std::string_view{s + start_idx, std::min(size() - start_idx, n_bytes)};
  }

  std::string_view str_view(std::size_t start_idx = 0) const
  {
    return str_view(start_idx, size());
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
    void *tmp = realloc(buf, new_size);
    if (tmp != nullptr)
    {
      buf = tmp;
      sz = new_size;
    }
  }

  /**
   * \brief Remove n bytes from the beginning of the buffer.
   *
   * Note that, if `n > size()`, the behavior is undefined.
   *
   * \param n How many bytes to be removed.
   */
  void remove_prefix(std::size_t n) noexcept;

  /**
   * \brief Remove n bytes from the end of the buffer.
   *
   * Note that, if `n > size()`, the behavior is undefined.
   *
   * \param n How many bytes to be removed.
   */
  void remove_suffix(std::size_t n) noexcept;

  friend void swap(Buffer &lhs, Buffer &rhs) noexcept
  {
    using std::swap;
    swap(lhs.sz, rhs.sz);
    swap(lhs.buf, rhs.buf);
  }

  bool operator==(const Buffer &other) const noexcept;
};

}  // namespace microloop
