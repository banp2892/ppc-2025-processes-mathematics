#include "chernykh_s_yadro_gaussa_horizontal/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

void GaussFilter(const std::vector<int> &local_data, std::vector<int> &local_res, int local_rows, int width,
                 int extended_rows, int halo_top) {
  for (int i = 0; i < local_rows; ++i) {
    int ext_i = i + halo_top;

    for (int j = 0; j < width; ++j) {
      int sum = 0;
      for (int ki = -1; ki <= 1; ++ki) {
        for (int kj = -1; kj <= 1; ++kj) {
          int stroka = std::clamp(ext_i + ki, 0, extended_rows - 1);
          int stolbec = std::clamp(j + kj, 0, width - 1);
          int weight;
          if (ki == 0 && kj == 0) {
            weight = 4;
          } else if (ki == 0 || kj == 0) {
            weight = 2;
          } else {
            weight = 1;
          }
          sum += local_data[stroka * width + stolbec] * weight;
        }
      }
      local_res[i * width + j] = sum / 16;
    }
  }
}

ChernykhSYadroGaussaHorizontalMPI::ChernykhSYadroGaussaHorizontalMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = std::numeric_limits<double>::max();
}

bool ChernykhSYadroGaussaHorizontalMPI::ValidationImpl() {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    auto &input = std::get<2>(GetInput());
    int w = std::get<0>(GetInput());
    int h = std::get<1>(GetInput());
    return w > 0 && h > 0 && input.size() == static_cast<size_t>(w * h);
  }
  return true;
}

bool ChernykhSYadroGaussaHorizontalMPI::PreProcessingImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int w, h;
  if (rank == 0) {
    w = std::get<0>(GetInput());
    h = std::get<1>(GetInput());
  }
  MPI_Bcast(&w, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&h, 1, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> strok_na_process(size);
  std::vector<int> offset(size);
  int current_offset = 0;
  for (int i = 0; i < size; ++i) {
    int count;
    if (i < (h % size)) {
      count = (h / size) + 1;
    } else {
      count = (h / size);
    }
    strok_na_process[i] = count;
    offset[i] = current_offset;
    current_offset = current_offset + count;
  }

  std::vector<int> sendcounts(size);
  std::vector<int> new_start(size);

  for (int i = 0; i < size; ++i) {
    int start_row = offset[i];
    int count_rows = strok_na_process[i];
    int h_top = (start_row > 0) ? 1 : 0;
    int h_bot = ((start_row + count_rows) < h) ? 1 : 0;
    sendcounts[i] = (count_rows + h_top + h_bot) * w;
    new_start[i] = (start_row - h_top) * w;
  }

  int local_rows = strok_na_process[rank];
  int halo_top = (offset[rank] > 0) ? 1 : 0;
  int halo_bot = ((offset[rank] + local_rows) < h) ? 1 : 0;
  int extended_rows = local_rows + halo_top + halo_bot;

  std::vector<int> local_data(extended_rows * w);

  const int *data_to_send = nullptr;
  int *send_counts_ptr = nullptr;
  int *new_start_ptr = nullptr;
  if (rank == 0) {
    data_to_send = std::get<2>(GetInput()).data();
    send_counts_ptr = sendcounts.data();
    new_start_ptr = new_start.data();
  }
  MPI_Scatterv(data_to_send, send_counts_ptr, new_start_ptr, MPI_INT, local_data.data(), extended_rows * w, MPI_INT, 0,
               MPI_COMM_WORLD);

  std::vector<int> local_res(local_rows * w);

  GaussFilter(local_data, local_res, local_rows, w, extended_rows, halo_top);

  std::vector<int> recvcounts(size);
  std::vector<int> recvdispls(size);
  int *recvcounts_ptr = nullptr;
  int *recvdispls_ptr = nullptr;
  int *final_output_ptr = nullptr;

  if (rank == 0) {
    GetOutput().resize(w * h);
    final_output_ptr = GetOutput().data();
    for (int i = 0; i < size; ++i) {
      recvcounts[i] = strok_na_process[i] * w;
      recvdispls[i] = offset[i] * w;
    }
    recvcounts_ptr = recvcounts.data();
    recvdispls_ptr = recvdispls.data();
  }

  MPI_Gatherv(local_res.data(), local_rows * w, MPI_INT, final_output_ptr, recvcounts_ptr, recvdispls_ptr, MPI_INT, 0,
              MPI_COMM_WORLD);

  return true;
}

bool ChernykhSYadroGaussaHorizontalMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_yadro_gaussa_horizontal
