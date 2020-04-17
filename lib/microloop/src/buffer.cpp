//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/buffer.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <new>

namespace microloop
{

Buffer::Buffer(std::size_t count) : data_{nullptr}, size_{count}
{
  if (size_)
  {
    data_ = std::make_unique<char[]>(size_);
  }
}

Buffer::Buffer(const char *str, std::size_t count) : Buffer{std::min(std::strlen(str), count)}
{
  if (size_)
  {
    std::copy_n(str, size_, data_.get());
  }
}

Buffer::Buffer(const Buffer &other) : Buffer{other.size_}
{
  if (size_)
  {
    std::copy_n(other.data_.get(), size_, data_.get());
  }
}

Buffer &Buffer::operator=(Buffer other)
{
  swap(*this, other);

  return *this;
}

std::string Buffer::str(std::size_t pos, std::size_t count) const
{
  return std::string{data_.get() + pos, std::min(size_ - pos, count)};
}

std::string_view Buffer::str_view(std::size_t pos, std::size_t count) const noexcept
{
  return std::string_view{data_.get() + pos, std::min(size_ - pos, count)};
}

Buffer::operator std::string_view() const noexcept
{
  return str_view();
}

bool Buffer::operator==(const Buffer &other) const noexcept
{
  if (size_ != other.size_)
  {
    return false;
  }

  return std::memcmp(data_.get(), other.data_.get(), size_) == 0;
}

void Buffer::resize(std::size_t new_size)
{
  if (new_size == size_)
  {
    return;
  }

  if (!new_size)
  {
    data_.reset();
    size_ = 0;
    return;
  }

  std::unique_ptr<char[]> next_data = std::make_unique<char[]>(new_size);
  if (size_)
  {
    std::copy_n(data_.get(), std::min(size_, new_size), next_data.get());
  }

  data_.swap(next_data);
  size_ = new_size;
}

Buffer &Buffer::remove_prefix(std::size_t count)
{
  if (size_ <= count)
  {
    clear();
    return *this;
  }

  std::size_t next_size = size_ - count;

  std::unique_ptr<char[]> next_data = std::make_unique<char[]>(next_size);
  std::copy_n(data_.get() + count, next_size, next_data.get());

  data_.swap(next_data);
  size_ = next_size;

  return *this;
}

Buffer &Buffer::remove_suffix(std::size_t count)
{
  if (size_ <= count)
  {
    clear();
    return *this;
  }

  resize(size_ - count);
  return *this;
}

void Buffer::clear() noexcept
{
  resize(0);
}

Buffer &Buffer::concat(const Buffer &other, std::size_t count)
{
  count = std::min(other.size_, count);

  auto prev_size = size_;

  resize(prev_size + count);
  std::copy_n(other.data_.get(), count, data_.get() + prev_size);

  return *this;
}

Buffer &Buffer::operator+=(const Buffer &other)
{
  concat(other);
  return *this;
}

}  // namespace microloop
