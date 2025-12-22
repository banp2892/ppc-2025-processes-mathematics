#pragma once

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"
#include "task/include/task.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

class ChernykhSYadroGaussaHorizontalMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ChernykhSYadroGaussaHorizontalMPI(const InType &in);
  static void GaussFilter(const std::vector<int> &local_data, std::vector<int> &local_res, int stroki_local, int stolbci, int halo_top);
int CalculateGauss(const std::vector<int> &data, int r, int c, int stolbci, int extended_rows);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};
}  // namespace chernykh_s_yadro_gaussa_horizontal
