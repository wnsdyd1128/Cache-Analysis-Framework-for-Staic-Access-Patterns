#pragma once

#include <string>

#include "ap/ApProgram.hpp"

namespace apex
{

/**
 * @brief LAT v2 JSON(문자열/파일)을 ApProgram으로 파싱한다.
 *
 * root는 `{schema_version, metadata, functions}` 객체이며, metadata.objects/
 * structs로 주소 해석에 필요한 배치 정보를, functions로 노드 트리를 복원한다.
 */
class ApLoader
{
public:
  /**
   * @brief LAT v2 JSON 문자열을 파싱한다.
   * @param json {schema_version,metadata,functions} 객체 JSON
   * @throws std::runtime_error JSON 형식이 LAT v2가 아닐 때
   */
  ApProgram load_program_string(const std::string & json) const;

  /** @brief LAT v2 JSON 파일을 파싱한다.
   * @throws std::runtime_error 파일을 열 수 없을 때 */
  ApProgram load_program_file(const std::string & path) const;
};

}  // namespace apex
