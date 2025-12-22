#include "chernykh_s_yadro_gaussa_horizontal/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <vector>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

int ChernykhSYadroGaussaHorizontalMPI::CalculateGauss(const std::vector<int> &data, int r, int c, int stolbci,
                                                      int extended_rows) {
  int pixel_sum = 0;
  for (int ki = -1; ki <= 1; ++ki) {
    for (int kj = -1; kj <= 1; ++kj) {
      int weight = 0;
      if (ki == 0 && kj == 0) {
        weight = 4;
      } else if (ki == 0 || kj == 0) {
        weight = 2;
      } else {
        weight = 1;
      }

      int cur_r = std::clamp(r + ki, 0, extended_rows - 1);
      int cur_c = std::clamp(c + kj, 0, stolbci - 1);
      size_t idx = static_cast<size_t>(cur_r) * static_cast<size_t>(stolbci) + static_cast<size_t>(cur_c);
      pixel_sum += data[idx] * weight;
    }
  }
  return pixel_sum / 16;
}

void ChernykhSYadroGaussaHorizontalMPI::GaussFilter(const std::vector<int> &local_data, std::vector<int> &local_res,
                                                    int stroki_local, int stolbci, int halo_top) {
  int extended_rows = static_cast<int>(local_data.size() / static_cast<size_t>(stolbci));
  for (int i = 0; i < stroki_local; ++i) {
    for (int j = 0; j < stolbci; ++j) {
      size_t res_idx = static_cast<size_t>(i) * static_cast<size_t>(stolbci) + static_cast<size_t>(j);
      local_res[res_idx] = CalculateGauss(local_data, i + halo_top, j, stolbci, extended_rows);
    }
  }
}

ChernykhSYadroGaussaHorizontalMPI::ChernykhSYadroGaussaHorizontalMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = std::vector<int>();
}

bool ChernykhSYadroGaussaHorizontalMPI::ValidationImpl() {
  int stolbci = std::get<0>(GetInput());
  int stroki = std::get<1>(GetInput());
  auto &input = std::get<2>(GetInput());
  return stolbci > 0 && stroki > 0 && input.size() == static_cast<size_t>(stroki) * static_cast<size_t>(stolbci);
}

bool ChernykhSYadroGaussaHorizontalMPI::PreProcessingImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int stolbci = 0;
  int stroki = 0;

  if (rank == 0) {
    stolbci = std::get<0>(GetInput());
    stroki = std::get<1>(GetInput());
  }

  MPI_Bcast(&stolbci, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&stroki, 1, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> strok_na_process(size, 0);
  std::vector<int> offset(size, 0);
  int current_offset = 0;

  for (int i = 0; i < size; ++i) {
    int count = (stroki / size) + (i < (stroki % size) ? 1 : 0);
    strok_na_process[i] = count;
    offset[i] = current_offset;
    current_offset += count;
  }

  std::vector<int> sendcounts(size, 0);
  std::vector<int> new_start(size, 0);

  for (int i = 0; i < size; ++i) {
    int start_row = offset[i];
    int count_rows = strok_na_process[i];
    int h_top = (start_row > 0) ? 1 : 0;
    int h_bot = ((start_row + count_rows) < stroki) ? 1 : 0;
    sendcounts[i] = (count_rows + h_top + h_bot) * stolbci;
    new_start[i] = (start_row - h_top) * stolbci;
  }

  int local_rows = strok_na_process[rank];
  int halo_top = (offset[rank] > 0) ? 1 : 0;
  int halo_bot = ((offset[rank] + local_rows) < stroki) ? 1 : 0;
  int extended_rows = local_rows + halo_top + halo_bot;

  std::vector<int> local_data(static_cast<size_t>(extended_rows) * static_cast<size_t>(stolbci));

  const int *data_to_send = nullptr;
  int *send_counts_ptr = nullptr;
  int *new_start_ptr = nullptr;

  if (rank == 0) {
    data_to_send = std::get<2>(GetInput()).data();
    send_counts_ptr = sendcounts.data();
    new_start_ptr = new_start.data();
  }

  MPI_Scatterv(data_to_send, send_counts_ptr, new_start_ptr, MPI_INT, local_data.data(), extended_rows * stolbci,
               MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> local_res(static_cast<size_t>(local_rows) * static_cast<size_t>(stolbci));
  GaussFilter(local_data, local_res, local_rows, stolbci, halo_top);

  int *final_output_ptr = nullptr;
  std::vector<int> recvcounts(size, 0);
  std::vector<int> recvdispls(size, 0);
  int *recvcounts_ptr = nullptr;
  int *recvdispls_ptr = nullptr;

  if (rank == 0) {
    GetOutput().resize(static_cast<size_t>(stolbci) * static_cast<size_t>(stroki));
    final_output_ptr = GetOutput().data();
    for (int i = 0; i < size; ++i) {
      recvcounts[i] = strok_na_process[i] * stolbci;
      recvdispls[i] = offset[i] * stolbci;
    }
    recvcounts_ptr = recvcounts.data();
    recvdispls_ptr = recvdispls.data();
  }

  MPI_Gatherv(local_res.data(), local_rows * stolbci, MPI_INT, final_output_ptr, recvcounts_ptr, recvdispls_ptr,
              MPI_INT, 0, MPI_COMM_WORLD);

  return true;
}

bool ChernykhSYadroGaussaHorizontalMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_yadro_gaussa_horizontal
