#include <gtest/gtest.h>
#include "ap/ApLoader.hpp"
#include "ap/ApNode.hpp"

using namespace apex;

// ── 헬퍼 ─────────────────────────────────────────────────────

static std::vector<std::unique_ptr<ApNode>> load(const std::string& json) {
    return ApLoader{}.load_json_string(json);
}

static const ArrayNode& as_array(const ApNode& n) {
    return static_cast<const ArrayNode&>(n);
}
static const ScalarNode& as_scalar(const ApNode& n) {
    return static_cast<const ScalarNode&>(n);
}
static const LoopNode& as_loop(const ApNode& n) {
    return static_cast<const LoopNode&>(n);
}
static const CallNode& as_call(const ApNode& n) {
    return static_cast<const CallNode&>(n);
}

// ── test_ap_loader: scalar ────────────────────────────────────

TEST(ApLoader, loads_scalar_with_op) {
    auto nodes = load(R"([{"type":"Scalar","name":"x","op":"load"}])");
    ASSERT_EQ(nodes.size(), 1u);
    ASSERT_EQ(nodes[0]->kind(), ApNodeKind::Scalar);
    const auto& s = as_scalar(*nodes[0]);
    EXPECT_EQ(s.name, "x");
    EXPECT_EQ(s.op,   "load");
}

// ── test_ap_loader: array ─────────────────────────────────────

TEST(ApLoader, loads_array_with_shape_from_json) {
    auto nodes = load(R"([{
        "type":"Array","name":"A",
        "indices":["i","j"],
        "shape":[100,100],
        "elem_size":4,
        "op":"store"
    }])");
    ASSERT_EQ(nodes.size(), 1u);
    ASSERT_EQ(nodes[0]->kind(), ApNodeKind::Array);
    const auto& a = as_array(*nodes[0]);
    EXPECT_EQ(a.name, "A");
    EXPECT_EQ(a.indices, (std::vector<std::string>{"i","j"}));
    EXPECT_EQ(a.shape,   (std::vector<int64_t>{100,100}));
    EXPECT_EQ(a.elem_size, 4);
    EXPECT_EQ(a.op, "store");
}

// ── test_ap_loader: loop ──────────────────────────────────────

TEST(ApLoader, loads_loop_nest_with_bound) {
    auto nodes = load(R"([{
        "type":"Loop","var":"i","start":0,"bound":100,"depth":1,"body":[]
    }])");
    ASSERT_EQ(nodes.size(), 1u);
    ASSERT_EQ(nodes[0]->kind(), ApNodeKind::Loop);
    const auto& l = as_loop(*nodes[0]);
    EXPECT_EQ(l.var,   "i");
    EXPECT_EQ(l.start, 0);
    EXPECT_EQ(l.bound, 100);
    EXPECT_EQ(l.depth, 1);
    EXPECT_EQ(l.trip_count(), 100);
}

TEST(ApLoader, loads_nested_loops) {
    auto nodes = load(R"([{
        "type":"Loop","var":"i","start":0,"bound":4,"depth":1,
        "body":[
            {"type":"Loop","var":"j","start":0,"bound":8,"depth":2,"body":[]}
        ]
    }])");
    ASSERT_EQ(nodes.size(), 1u);
    const auto& outer = as_loop(*nodes[0]);
    ASSERT_EQ(outer.body.size(), 1u);
    const auto& inner = as_loop(*outer.body[0]);
    EXPECT_EQ(inner.var,   "j");
    EXPECT_EQ(inner.bound, 8);
    EXPECT_EQ(inner.depth, 2);
}

// ── test_ap_loader: call ──────────────────────────────────────

TEST(ApLoader, loads_call_stmt) {
    auto nodes = load(R"([{"type":"Call","callee":"foo","args":["A","B"]}])");
    ASSERT_EQ(nodes.size(), 1u);
    ASSERT_EQ(nodes[0]->kind(), ApNodeKind::Call);
    const auto& c = as_call(*nodes[0]);
    EXPECT_EQ(c.callee, "foo");
    EXPECT_EQ(c.args, (std::vector<std::string>{"A","B"}));
}
