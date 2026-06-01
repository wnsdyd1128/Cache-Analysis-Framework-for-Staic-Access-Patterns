#include "analysis/Diagnostics.hpp"
#include <algorithm>

namespace apex {

Diagnostics::Diagnostics(double threshold) : threshold_(threshold) {}

std::vector<DiagnosticHint> Diagnostics::generate(const MissStats& stats) const {
    std::vector<DiagnosticHint> hints;

    uint64_t total_typed = stats.cold + stats.capacity + stats.conflict;
    uint64_t total_op    = stats.load + stats.store;

    if (total_typed > 0) {
        double conflict_ratio  = static_cast<double>(stats.conflict)  / total_typed;
        double capacity_ratio  = static_cast<double>(stats.capacity)  / total_typed;

        if (conflict_ratio > threshold_) {
            hints.push_back({"conflict_padding",
                             "conflict miss 비중 높음: padding/layout 검토", ""});
        }
        if (capacity_ratio > threshold_) {
            hints.push_back({"capacity_blocking",
                             "capacity miss 비중 높음: blocking/tiling 검토", ""});
        }
    }

    if (total_op > 0) {
        double store_ratio = static_cast<double>(stats.store) / total_op;
        if (store_ratio > threshold_) {
            hints.push_back({"store_write_policy",
                             "store miss 비중 높음: write policy 검토", ""});
        }
    }

    uint64_t total_obj = 0;
    for (auto& [name, cnt] : stats.by_object) total_obj += cnt;

    if (total_obj > 0) {
        auto max_it = std::max_element(
            stats.by_object.begin(), stats.by_object.end(),
            [](const auto& a, const auto& b){ return a.second < b.second; });

        if (static_cast<double>(max_it->second) / total_obj > threshold_) {
            hints.push_back({"object_targeted",
                             max_it->first + " 우선 최적화 검토",
                             max_it->first});
        }
    }

    return hints;
}

}  // namespace apex
