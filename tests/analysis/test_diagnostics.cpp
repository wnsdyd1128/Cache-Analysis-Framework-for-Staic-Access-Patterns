#include <gtest/gtest.h>

#include "analysis/Diagnostics.hpp"

// Diagnostics::generate(stats) → 규칙 비중이 threshold를 초과하면 해당 hint
// 생성. 기본 threshold = 0.5 (50%).

TEST(Diagnostics, conflict_miss_dominant_triggers_padding_hint)
{
  apex::MissStats stats;
  stats.conflict = 10;  // 100% conflict
  apex::Diagnostics diag;
  auto hints = diag.generate(stats);
  ASSERT_FALSE(hints.empty());
  auto it = std::find_if(hints.begin(), hints.end(),
                         [](const apex::DiagnosticHint & h) {
                           return h.kind == "conflict_padding";
                         });
  EXPECT_NE(it, hints.end());
}

TEST(Diagnostics, capacity_miss_dominant_triggers_blocking_hint)
{
  apex::MissStats stats;
  stats.capacity = 10;  // 100% capacity
  apex::Diagnostics diag;
  auto hints = diag.generate(stats);
  auto it = std::find_if(hints.begin(), hints.end(),
                         [](const apex::DiagnosticHint & h) {
                           return h.kind == "capacity_blocking";
                         });
  EXPECT_NE(it, hints.end());
}

TEST(Diagnostics, store_miss_dominant_triggers_write_policy_hint)
{
  apex::MissStats stats;
  stats.store = 10;  // 100% store (load=0)
  apex::Diagnostics diag;
  auto hints = diag.generate(stats);
  auto it = std::find_if(hints.begin(), hints.end(),
                         [](const apex::DiagnosticHint & h) {
                           return h.kind == "store_write_policy";
                         });
  EXPECT_NE(it, hints.end());
}

TEST(Diagnostics, single_object_dominates_triggers_targeted_hint)
{
  apex::MissStats stats;
  stats.by_object["A"] = 9;
  stats.by_object["B"] = 1;  // A가 90% 지배
  apex::Diagnostics diag;
  auto hints = diag.generate(stats);
  auto it = std::find_if(
    hints.begin(), hints.end(),
    [](const apex::DiagnosticHint & h) { return h.kind == "object_targeted"; });
  ASSERT_NE(it, hints.end());
  EXPECT_EQ(it->object, "A");
}