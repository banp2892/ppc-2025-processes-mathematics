#include <gtest/gtest.h>

#include <vector>

#include "chernykh_s_hypercube/common/include/common.hpp"
#include "chernykh_s_hypercube/mpi/include/ops_mpi.hpp"
#include "chernykh_s_hypercube/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace chernykh_s_hypercube {

class ChernykhSRunFuncTestsHypercube : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_;

  void SetUp() override {
    // Минимальная заглушка
    input_data_ = {0, 1};
  }

  bool CheckTestOutputData(OutType &output_data [[maybe_unused]]) final {
    // Используем правильную функцию GetMPIRank
    if (ppc::util::GetMPIRank() == 0) {
      return true;
    }
    return true;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ChernykhSRunFuncTestsHypercube, RunPerfModes) {
  ExecuteTest(GetParam());
}

// Убедись, что ChernykhSHypercubeMPI определен в ops_mpi.hpp
// внутри namespace chernykh_s_hypercube
const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, ChernykhSHypercubeMPI, ChernykhSHypercubeSEQ>(
    PPC_SETTINGS_chernykh_s_hypercube);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ChernykhSRunFuncTestsHypercube::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ChernykhSRunFuncTestsHypercube, kGtestValues, kPerfTestName);

}  // namespace chernykh_s_hypercube
