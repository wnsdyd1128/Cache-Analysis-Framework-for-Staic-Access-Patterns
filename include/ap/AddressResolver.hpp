#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "ap/AccessLayout.hpp"

namespace apex
{

/**
 * @brief 평가된 access_path를 객체-상대 byte offset으로 환산한다.
 *
 * 절대 주소가 아니라 객체 base 기준 오프셋을 반환한다
 * (절대 주소 = base(object) + 반환값; base는 Pipeline의 MemoryLayout가 결정).
 * 연속된 index 스텝은 trailing 정렬 row-major로, field 스텝은 구조체 offset으로 누적한다.
 *
 * @param obj     접근 base 객체의 타입 배치
 * @param path    평가된 access_path (index 값은 정수로 채워져 있어야 함)
 * @param structs 구조체 이름 → 배치. field 스텝이 있으면 필요.
 * @return 객체 base 기준 byte offset
 * @throws std::out_of_range  field 스텝의 struct_type/field_index가 structs에 없을 때
 */
int64_t resolve_offset(const ObjectLayout& obj,
                       const std::vector<AccessStep>& path,
                       const std::map<std::string, StructLayout>& structs);

}  // namespace apex
