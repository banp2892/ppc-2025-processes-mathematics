#include "chernykh_s_yadro_gaussa_horizontal/seq/include/ops_seq.hpp"

#include <algorithm>
#include <limits>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

ChernykhSYadroGaussaHorizontalSEQ::ChernykhSYadroGaussaHorizontalSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
}

bool ChernykhSYadroGaussaHorizontalSEQ::ValidationImpl() {
  int w = std::get<0>(GetInput());
  int h = std::get<1>(GetInput());
  auto &data = std::get<2>(GetInput());
  return w > 0 && h > 0 && data.size() == static_cast<size_t>(w * h);
}
bool ChernykhSYadroGaussaHorizontalSEQ::PreProcessingImpl() {
  return true;
}

bool ChernykhSYadroGaussaHorizontalSEQ::RunImpl() {
  int w = std::get<0>(GetInput());
  int h = std::get<1>(GetInput());
  std::vector<int> &input_data = std::get<2>(GetInput());
  std::vector<int> &output_data = GetOutput();
  output_data.resize(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int sum = 0;
      for (int ki = -1; ki <= 1; ++ki) {
        for (int kj = -1; kj <= 1; ++kj) {
          int stroka = std::clamp(i + ki, 0, h - 1);
          int stolbec = std::clamp(j + kj, 0, w - 1);

          int weight;
          if (ki == 0 && kj == 0) {
            weight = 4;
          } else if (ki == 0 || kj == 0) {
            weight = 2;
          } else {
            weight = 1;
          }

          sum += input_data[stroka * w + stolbec] * weight;
        }
      }
      output_data[i * w + j] = sum / 16;
    }
  }
  return true;
}

bool ChernykhSYadroGaussaHorizontalSEQ::PostProcessingImpl() {
  return true;
  ;
}

}  // namespace chernykh_s_yadro_gaussa_horizontal
