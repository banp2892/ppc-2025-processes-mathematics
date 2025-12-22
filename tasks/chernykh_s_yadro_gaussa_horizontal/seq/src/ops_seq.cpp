#include "chernykh_s_yadro_gaussa_horizontal/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <vector>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

int ChernykhSYadroGaussaHorizontalSEQ::GetGaussianWeight(int ki, int kj) {
  if (ki == 0 && kj == 0) {
    return 4;
  }
  if (ki == 0 || kj == 0) {
    return 2;
  }
  return 1;
}

ChernykhSYadroGaussaHorizontalSEQ::ChernykhSYadroGaussaHorizontalSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
}

bool ChernykhSYadroGaussaHorizontalSEQ::ValidationImpl() {
  int stolbci = std::get<0>(GetInput());
  int stroki = std::get<1>(GetInput());
  auto &data = std::get<2>(GetInput());
  // Приведение к size_t предотвращает переполнение при умножении
  return stolbci > 0 && stroki > 0 && data.size() == static_cast<size_t>(stolbci) * static_cast<size_t>(stroki);
}

bool ChernykhSYadroGaussaHorizontalSEQ::PreProcessingImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalSEQ::RunImpl() {
  int stolbci = std::get<0>(GetInput());
  int stroki = std::get<1>(GetInput());
  std::vector<int> &input_data = std::get<2>(GetInput());
  std::vector<int> &output_data = GetOutput();

  output_data.resize(static_cast<size_t>(stolbci) * static_cast<size_t>(stroki));

  for (int i = 0; i < stroki; ++i) {
    for (int j = 0; j < stolbci; ++j) {
      int sum = 0;
      for (int ki = -1; ki <= 1; ++ki) {
        for (int kj = -1; kj <= 1; ++kj) {
          int row = std::clamp(i + ki, 0, stroki - 1);
          int column = std::clamp(j + kj, 0, stolbci - 1);
          size_t idx = static_cast<size_t>(row) * static_cast<size_t>(stolbci) + static_cast<size_t>(column);
          sum += input_data[idx] * GetGaussianWeight(ki, kj);
        }
      }
      size_t out_idx = static_cast<size_t>(i) * static_cast<size_t>(stolbci) + static_cast<size_t>(j);
      output_data[out_idx] = sum / 16;
    }
  }
  return true;
}

bool ChernykhSYadroGaussaHorizontalSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace chernykh_s_yadro_gaussa_horizontal
