#include "chernykh_s_hypercube/seq/include/ops_seq.hpp"

#include <algorithm>
#include <limits>

#include "chernykh_s_hypercube/common/include/common.hpp"

namespace chernykh_s_hypercube {

ChernykhSHypercubeSEQ::ChernykhSHypercubeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = std::numeric_limits<double>::max();
}

bool ChernykhSHypercubeSEQ::ValidationImpl() {
  return (GetOutput() == std::numeric_limits<double>::max());
}

bool ChernykhSHypercubeSEQ::PreProcessingImpl() {
  return true;
}

bool ChernykhSHypercubeSEQ::RunImpl() {
  const auto &matrix = GetInput();

  if (matrix.empty()) {
    GetOutput() = std::numeric_limits<double>::max();
    return true;
  }
  double minimum = std::numeric_limits<double>::max();
  for (const auto &row : matrix) {
    for (double element : row) {
      minimum = std::min(element, minimum);
    }
  }

  GetOutput() = minimum;
  return true;
}

bool ChernykhSHypercubeSEQ::PostProcessingImpl() {
  return true;
  ;
}

}  // namespace chernykh_s_hypercube
