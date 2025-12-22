#include <gtest/gtest.h>
#include <mpi.h>

#include <algorithm>
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
  size_t expected_output_size_ = 0;
  int size = 2000; 

 protected:
  void SetUp() override {
    int h = size;
    int w = size;
    unsigned int seed = 999;
    std::mt19937 gen(seed); 
    std::uniform_int_distribution<int> dist(0, 255);
    std::vector<int> pixels(h * w);
    for (int i = 0; i < h * w; i++) {
      pixels[i] = dist(gen);
    }
    input_data_ = std::make_tuple(w, h, pixels);
    expected_output_size_ = static_cast<size_t>(h) * static_cast<size_t>(w);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int rank = 0;
    int is_mpi_active = 0;
    MPI_Initialized(&is_mpi_active);
    if (is_mpi_active) {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    }
    if (rank != 0 && output_data.empty()) {
        return true;
    }
    if (output_data.size() != expected_output_size_) {
        return false;
    }
    for (int i = 0; i < output_data.size(); ++i) {
      int val = output_data[i];
      if (val < 0 || val > 255) {
          return false;
      }
    }
    return true;
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

}  // namespace chernykh_s_yadro_gaussa_horizontal