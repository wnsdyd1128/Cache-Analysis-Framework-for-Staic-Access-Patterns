#include "memory/AddressMapper.hpp"

uint64_t AddressMapper::cache_line(uint64_t byte_addr, uint64_t line_size)
{
  return byte_addr / line_size;
}
