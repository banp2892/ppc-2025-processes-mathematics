#pragma once

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"
#include "task/include/task.hpp"  // мб не надо?

namespace chernykh_s_yadro_gaussa_horizontal {

class ChernykhSYadroGaussaHorizontalSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ChernykhSYadroGaussaHorizontalSEQ(const InType &in);
  static int GetGaussianWeight(int ki, int kj);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace chernykh_s_yadro_gaussa_horizontal
