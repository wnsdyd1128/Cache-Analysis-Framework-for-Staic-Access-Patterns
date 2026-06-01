#include <gtest/gtest.h>

#include "memory/MemoryLayout.hpp"

TEST(MemoryLayout, first_object_base_is_zero)
{
  MemoryLayout layout;
  layout.add_object("A", 400);  // 100 * 4
  EXPECT_EQ(layout.base_of("A"), 0u);
}

TEST(MemoryLayout, second_object_starts_aligned_after_first)
{
  MemoryLayout layout;
  layout.add_object("A", 400);  // 100 * 4 = 400 bytes
  layout.add_object("B", 400);
  // 기본 alignment=32: align_up(400) = 416
  EXPECT_EQ(layout.base_of("B"), 416u);
}

TEST(MemoryLayout, scalar_occupies_element_size_bytes)
{
  MemoryLayout layout;
  layout.add_object("x", 4);  // scalar: element_size = 4
  layout.add_object("y", 4);
  // 기본 alignment=32: align_up(4) = 32
  EXPECT_EQ(layout.base_of("y"), 32u);
}

TEST(MemoryLayout, all_objects_64_byte_aligned)
{
  MemoryLayout layout;
  layout.add_object("A", 100);
  layout.add_object("B", 300);
  layout.add_object("C", 500);
  EXPECT_EQ(layout.base_of("A") % 64, 0u);
  EXPECT_EQ(layout.base_of("B") % 64, 0u);
  EXPECT_EQ(layout.base_of("C") % 64, 0u);
}

TEST(MemoryLayout, custom_alignment_applied_from_constructor)
{
  // line_size=128 캐시 설정에서는 128바이트 정렬을 주입해야 한다.
  MemoryLayout layout(128);
  layout.add_object("A", 100);
  layout.add_object("B", 100);
  // align_up(100, 128) = 128
  EXPECT_EQ(layout.base_of("B"), 128u);
  EXPECT_EQ(layout.base_of("B") % 128, 0u);
}
