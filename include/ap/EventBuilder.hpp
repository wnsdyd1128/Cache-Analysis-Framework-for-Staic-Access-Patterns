#pragma once

#include "ap/AccessEvent.hpp"
#include "ap/ApNode.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace apex {

/**
 * @brief ApNode 트리를 선형 AccessEvent 스트림으로 변환한다.
 *
 * 루프는 trip_count만큼 unroll하고, CallStmt는 callee AP를
 * 재귀 전개한다. 각 이벤트의 byte_address/cache_line은
 * Phase 3(Memory Layer)에서 채운다.
 *
 * @note SMP core_id 기본값은 0이다. mapping.yaml 적용은 CLI에서 처리한다.
 */
class EventBuilder {
public:
    /**
     * @brief callee 이름 → 해당 함수의 ApNode 목록 매핑을 등록한다.
     *
     * CallStmt 전개 시 조회에 사용한다.
     * @param name    함수 이름
     * @param nodes   해당 함수의 최상위 ApNode 목록
     */
    EventBuilder& register_function(
        const std::string& name,
        std::vector<std::unique_ptr<ApNode>> nodes);

    /**
     * @brief ApNode 목록을 AccessEvent 스트림으로 변환한다.
     * @param nodes 최상위 ApNode 목록
     * @return sequence_id 순으로 정렬된 AccessEvent 목록
     */
    std::vector<AccessEvent> build(
        std::vector<std::unique_ptr<ApNode>> nodes);

private:
    using FuncMap = std::unordered_map<
        std::string,
        std::vector<std::unique_ptr<ApNode>>>;

    FuncMap func_map_;

    void visit(const ApNode& node,
               std::vector<AccessEvent>& out,
               std::vector<LoopFrame>&   loop_stack,
               const std::string&        region_path,
               uint64_t&                 seq);
};

}  // namespace apex
