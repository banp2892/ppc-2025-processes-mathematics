#include <gtest/gtest.h>

#include <cmath>
#include <random>
#include <vector>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"
#include "chernykh_s_yadro_gaussa_horizontal/mpi/include/ops_mpi.hpp"
#include "chernykh_s_yadro_gaussa_horizontal/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"
namespace chernykh_s_yadro_gaussa_horizontal {

class ChernykhSRunFuncTestsGaussaHorizontal : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_;

  static InType GenerateMatrix(int n) {
    InType matrix;
    int seed = 999;
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(-500.0, 500.0);
    for (int i = 0; i < n; i++) {
      std::vector<double> row;
      row.reserve(n);
      for (int j = 0; j < n; j++) {
        row.push_back(distribution(generator));
      }
      matrix.push_back(row);
    }
    matrix[n / 2][n / 2] = -1000.0;
    return matrix;
  }

  void SetUp() override {
    input_data_ = GenerateMatrix(8192);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return std::fabs(output_data + 1000.0) < 1e-6;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ChernykhSRunFuncTestsGaussaHorizontal, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ChernykhSYadroGaussaHorizontalMPI, ChernykhSYadroGaussaHorizontalSEQ>(
        PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ChernykhSRunFuncTestsGaussaHorizontal::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ChernykhSRunFuncTestsGaussaHorizontal, kGtestValues, kPerfTestName);

};  // namespace chernykh_s_yadro_gaussa_horizontal
