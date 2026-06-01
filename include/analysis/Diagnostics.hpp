#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace apex {

struct MissStats {
    uint64_t cold     = 0;
    uint64_t capacity = 0;
    uint64_t conflict = 0;
    uint64_t load     = 0;
    uint64_t store    = 0;
    std::unordered_map<std::string, uint64_t> by_object;
};

struct DiagnosticHint {
    std::string kind;     ///< "conflict_padding" | "capacity_blocking" | "store_write_policy" | "object_targeted"
    std::string message;  ///< 사람이 읽는 설명
    std::string object;   ///< object_targeted 힌트에서 대상 객체명
};

/**
 * @brief MissStats를 rule-based로 분석해 최적화 힌트를 생성한다.
 *
 * 각 규칙은 해당 miss 비중이 threshold를 초과할 때 트리거된다.
 *
 * @param threshold  힌트 트리거 비중 임계값 (기본 0.5)
 */
class Diagnostics {
public:
    explicit Diagnostics(double threshold = 0.5);

    std::vector<DiagnosticHint> generate(const MissStats& stats) const;

private:
    double threshold_;
};

}  // namespace apex
