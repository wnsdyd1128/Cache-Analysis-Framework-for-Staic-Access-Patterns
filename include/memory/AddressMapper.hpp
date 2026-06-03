#pragma once
#include <cstdint>

/**
 * @brief byte address ↔ cache line 변환 유틸리티.
 */
class AddressMapper
{
public:
  /**
   * @brief byte address를 cache line 번호로 변환한다.
   *
   * @param[in] byte_addr byte address
   * @param[in] line_size cache line 크기 (bytes)
   * @return cache line 번호 (= byte_addr / line_size)
   */
  static uint64_t cache_line(uint64_t byte_addr, uint64_t line_size);
};
