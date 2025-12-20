#include "chernykh_s_yadro_gaussa_horizontal/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

ChernykhSYadroGaussaHorizontalMPI::ChernykhSYadroGaussaHorizontalMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = std::numeric_limits<double>::max();
}

bool ChernykhSYadroGaussaHorizontalMPI::ValidationImpl() {
  return (GetOutput() == std::numeric_limits<double>::max());
}

bool ChernykhSYadroGaussaHorizontalMPI::PreProcessingImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalMPI::RunImpl() {
  
  return true;
}

bool ChernykhSYadroGaussaHorizontalMPIMPIMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_yadro_gaussa_horizontal
