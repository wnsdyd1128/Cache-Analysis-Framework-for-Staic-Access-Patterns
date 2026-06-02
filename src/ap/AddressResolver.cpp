#include "ap/AddressResolver.hpp"

#include <cstddef>

namespace apex
{

int64_t resolve_offset(const ObjectLayout& obj,
                       const std::vector<AccessStep>& path,
                       const std::map<std::string, StructLayout>& structs)
{
  // 현재 타입 컨텍스트: 남은 배열 차원 + 원소 크기 + (원소가 구조체면) 그 이름.
  std::vector<int64_t> shape = obj.shape;
  int64_t elem_size = obj.elem_size;
  std::string struct_type = obj.struct_type;

  int64_t offset = 0;
  std::size_t i = 0;
  while (i < path.size())
  {
    if (path[i].kind == AccessStep::Kind::Field)
    {
      const StructLayout& st = structs.at(struct_type);
      const FieldLayout& f = st.fields.at(static_cast<std::size_t>(path[i].field_index));
      offset += f.offset;
      shape = f.shape;
      elem_size = f.elem_size;
      struct_type = f.struct_type;
      ++i;
      continue;
    }

    // 연속된 index 스텝을 모아 trailing 정렬 row-major로 누적.
    std::vector<int64_t> idxs;
    while (i < path.size() && path[i].kind == AccessStep::Kind::Index)
    {
      idxs.push_back(path[i].index_value);
      ++i;
    }

    int64_t stride = elem_size;
    int si = static_cast<int>(shape.size());
    for (std::size_t p = idxs.size(); p-- > 0;)
    {
      offset += idxs[p] * stride;
      --si;
      if (si >= 0) stride *= shape[static_cast<std::size_t>(si)];
    }
    shape.clear();  // 배열 차원 소진; 원소 타입(struct_type/elem_size)은 유지
  }

  return offset;
}

}  // namespace apex
