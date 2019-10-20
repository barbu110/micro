//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/buffer.h"

namespace microloop
{

bool Buffer::operator==(const Buffer &other) const
{
  if (size() != other.size())
  {
    return false;
  }

  const std::uint8_t *a = static_cast<const std::uint8_t *>(data());
  const std::uint8_t *b = static_cast<const std::uint8_t *>(other.data());

  for (std::size_t i = 0; i < size(); i++)
  {
    if (a[i] != b[i])
    {
      return false;
    }
  }

  return true;
}

void Buffer::remove_prefix(std::size_t n) noexcept
{
  buf = std::memmove(buf, static_cast<std::uint8_t *>(buf) + n, size() - n);
  sz -= n;
}

void Buffer::remove_suffix(std::size_t n) noexcept
{
  resize(size() - n);
}

}  // namespace microloop
