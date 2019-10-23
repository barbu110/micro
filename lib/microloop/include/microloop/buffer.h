//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace microloop
{

class Buffer
{
public:
  /**
   * \brief Create a buffer of a given size, filled with zeros.
   * \param count How many bytes to be allocated and zeroed out.
   */
  Buffer(std::size_t count = 0);

  /**
   * \brief Create a buffer from a C string, or substring. Note that, the NUL byte at the end of a
   * C string is not included in the buffer if the \p count parameter is omitted.
   * \param str The C string to be used
   * \param count The number of bytes to be included of the given string. If omitted, the entire
   * string will be used (without the NUL byte).
   */
  Buffer(const char *str, std::size_t count = -1);

  /**
   * \brief Copy constructor.
   */
  Buffer(const Buffer &other);

  /**
   * \brief Move constructor.
   */
  Buffer(Buffer &&other) noexcept;

  /**
   * \brief Copy/move assignment operator. Implemented using the copy-and-swap idiom.
   */
  Buffer &operator=(Buffer other);

  friend void swap(Buffer &a, Buffer &b)
  {
    using std::swap;

    swap(a.data_, b.data_);
    swap(a.size_, b.size_);
  }

  /**
   * \brief Get the raw underlying data.
   */
  void *data() noexcept
  {
    return static_cast<void *>(data_);
  }

  const void *data() const noexcept
  {
    return static_cast<const void *>(data_);
  }

  /**
   * \brief Get the size of the buffer.
   */
  std::size_t size() const noexcept
  {
    return size_;
  }

  bool empty() const noexcept
  {
    return size_ == 0;
  }

  /**
   * \brief Construct a `std::string` out of this buffer. Note that construction of this string is
   * done without any guarantees that the string will include a NUL byte or not.
   * \param pos The starting offset for the resulting string. By default, the string will start at
   * offset 0.
   * \param count How many bytes to use in the resulting string.
   */
  std::string str(std::size_t pos = 0, std::size_t count = -1) const;

  /**
   * \brief Get a string view out of this buffer.
   * \param pos The offset into the buffer that will be contained by the resulting string view.
   * \param count How many bytes to be put into the resulting string view.
   */
  std::string_view str_view(std::size_t pos = 0, std::size_t count = -1) const noexcept;

  /**
   * \brief Convert this buffer to a string view.
   */
  operator std::string_view() const noexcept;

  /**
   * \brief Compare this buffer to another. The comparison is done byte by byte.
   */
  bool operator==(const Buffer &other) const noexcept;

  /**
   * \brief Resize this buffer to the given size.
   * \param new_size The new size of the buffer. If the size is greater than the current size, then
   * the contents of the trailing memory will be zeroed out.
   */
  void resize(std::size_t new_size);

  /**
   * \brief Remove \p count bytes from the beginning of this buffer.
   * \param count How many bytes to be removed. If this is higher than \p size(), then the behavior
   * is undefined.
   */
  Buffer &remove_prefix(std::size_t count);

  /**
   * \brief Remove \p count bytes from the end of this buffer.
   * \param count How many bytes to be removed. If this is higher than \p size(), then the behavior
   * is undefined.
   */
  Buffer &remove_suffix(std::size_t count);

  /**
   * \brief Release all the underlying memory of this buffer.
   */
  void clear() noexcept;

  /**
   * \brief Concatenate another buffer to the end of this one.
   * \param other The buffer to be concatenated.
   * \param count How many bytes to be taken from the beginning of the \p other buffer.
   */
  Buffer &concat(const Buffer &other, std::size_t count = -1);

  /**
   * \brief Concatenate another buffer to the end of this one.
   */
  Buffer &operator+=(const Buffer &other);

  ~Buffer()
  {
    delete[] data_;
  }

private:
  char *data_;
  std::size_t size_;
};

}  // namespace microloop
