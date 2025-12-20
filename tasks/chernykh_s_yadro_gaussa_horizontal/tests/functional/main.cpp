#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <ios>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "chernykh_s_yadro_gaussa_horizontal/common/include/common.hpp"
#include "chernykh_s_yadro_gaussa_horizontal/mpi/include/ops_mpi.hpp"
#include "chernykh_s_yadro_gaussa_horizontal/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

class ChernykhSRunFuncTestsGaussaHorizontal : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  void SetUp() override {
    
  }
  bool CheckTestOutputData(OutType &output_data) final {
    
    return std::fabs(output_data - expected_min) < 1e-6;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(ChernykhSRunFuncTestsGaussaHorizontal, SumHypercube) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 0> kTestParam = {};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ChernykhSYadroGaussaHorizontal, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal),
                                           ppc::util::AddFuncTask<ChernykhSYadroGaussaHorizontalSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    ChernykhSRunFuncTestsGaussaHorizontal::PrintFuncTestName<ChernykhSRunFuncTestsGaussaHorizontal>;

INSTANTIATE_TEST_SUITE_P(HypercubeTests, ChernykhSRunFuncTestsGaussaHorizontal, kGtestValues, kPerfTestName);

}  // namespace
}  // namespace chernykh_s_yadro_gaussa_horizontal
