#include "chernykh_s_hypercube/seq/include/ops_seq.hpp"

#include <algorithm>
#include <limits>

#include "chernykh_s_hypercube/common/include/common.hpp"

namespace chernykh_s_hypercube {

ChernykhSHypercubeSEQ::ChernykhSHypercubeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = 0;
}

bool ChernykhSHypercubeSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool ChernykhSHypercubeSEQ::PreProcessingImpl() {
  return true;
}

bool ChernykhSHypercubeSEQ::RunImpl() {
  const std::vector<int> &active_nodes = GetInput();
  int total_sum = std::accumulate(active_nodes.begin(), active_nodes.end(), 0);
  GetOutput() = total_sum;
  return true;
}

bool ChernykhSHypercubeSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace chernykh_s_hypercube
