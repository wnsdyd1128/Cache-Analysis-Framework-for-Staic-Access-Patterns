#include <gtest/gtest.h>

#include "memory/AddressMapper.hpp"

TEST(AddressMapper, cache_line_equals_address_div_line_size)
{
  uint64_t line = AddressMapper::cache_line(128, 64);
  EXPECT_EQ(line, 2u);
}
