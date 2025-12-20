#include "chernykh_s_yadro_gaussa_horizontal/seq/include/ops_seq.hpp"

#include <algorithm>
#include <limits>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

ChernykhSYadroGaussaHorizontalSEQ::ChernykhSYadroGaussaHorizontalSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = std::numeric_limits<double>::max();
}

bool ChernykhSYadroGaussaHorizontalSEQ::ValidationImpl() {
  return (GetOutput() == std::numeric_limits<double>::max());
}

bool ChernykhSYadroGaussaHorizontalSEQ::PreProcessingImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalSEQ::RunImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalSEQ::PostProcessingImpl() {
  return true;
  ;
}

}  // namespace chernykh_s_yadro_gaussa_horizontal
