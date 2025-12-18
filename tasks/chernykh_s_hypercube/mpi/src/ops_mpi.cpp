#include "chernykh_s_hypercube/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

#include "chernykh_s_hypercube/common/include/common.hpp"

static bool process_hypercube_step(int &local_data, int step, int rank, int size) {
  int mask = 1 << step;
  int neighbor = rank ^ mask;

  if ((rank & (mask - 1)) != 0) {
    return false;
  }

  if (rank & mask) {
    MPI_Send(&local_data, 1, MPI_INT, neighbor, 0, MPI_COMM_WORLD);
    return false;
  } else {
    if (neighbor < size) {
      int received_val = 0;
      MPI_Recv(&received_val, 1, MPI_INT, neighbor, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      local_data += received_val;
    }
    return true;
  }
}

namespace chernykh_s_hypercube {

ChernykhSHypercubeMPI::ChernykhSHypercubeMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = 0;
}

bool ChernykhSHypercubeMPI::ValidationImpl() {
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  bool is_power_of_two = (size > 0) && ((size & (size - 1)) == 0);
  if (!is_power_of_two) {
    return false;
  }

  if (GetInput().empty()) {
    return false;
  }

  for (int r : GetInput()) {
    if (r < 0 || r >= size) {
      return false;
    }
  }

  return true;
}

bool ChernykhSHypercubeMPI::PreProcessingImpl() {
  return true;
}

bool ChernykhSHypercubeMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::vector<int> &active_nodes = GetInput();

  int current_val = 0;
  if (std::find(active_nodes.begin(), active_nodes.end(), rank) != active_nodes.end()) {
    current_val = rank;
  }

  int dims = 0;
  while ((1 << dims) < size) {
    dims++;
  }

  for (int i = 0; i < dims; ++i) {
    if (!process_hypercube_step(current_val, i, rank, size)) {
      break;
    }
  }
  if (rank == 0) {
    GetOutput() = current_val;
  }

  return true;
}

bool ChernykhSHypercubeMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_hypercube
