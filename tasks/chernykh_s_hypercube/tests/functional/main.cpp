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

#include "chernykh_s_hypercube/common/include/common.hpp"
#include "chernykh_s_hypercube/mpi/include/ops_mpi.hpp"
#include "chernykh_s_hypercube/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace chernykh_s_hypercube {

class ChernykhSRunFuncTestsHypercube : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());

    std::string in_file_name = params + ".txt";
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_chernykh_s_hypercube, in_file_name);

    std::ifstream in_file(abs_path, std::ios::in);
    if (!in_file.is_open()) {
      throw std::runtime_error("Failed to open file : " + abs_path);
    }

    input_data_.clear();
    std::string line;

    while (std::getline(in_file, line)) {
      if (line.empty()) {
        continue;
      }

      std::istringstream iss(line);
      std::vector<double> row;
      double value = 0.0;

      while (iss >> value) {
        while (iss >> value) {
          row.push_back(value);
        }

        if (!row.empty()) {
          input_data_.push_back(row);
        }
      }
    }
  }
  bool CheckTestOutputData(OutType &output_data) final {
    const auto &mat = input_data_;
    double expected_min = std::numeric_limits<double>::max();
    for (const auto &row : mat) {
      for (double v : row) {
        expected_min = std::min(expected_min, v);
      }
    }
    return std::fabs(output_data - expected_min) < 1e-6;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(ChernykhSRunFuncTestsHypercube, FindMinInMatrix) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 6> kTestParam = {};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ChernykhSHypercubeMPI, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_hypercube),
                                           ppc::util::AddFuncTask<ChernykhSHypercubeSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_hypercube));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    ChernykhSRunFuncTestsHypercube::PrintFuncTestName<ChernykhSRunFuncTestsHypercube>;

INSTANTIATE_TEST_SUITE_P(MinMatrixTests, ChernykhSRunFuncTestsHypercube, kGtestValues, kPerfTestName);

}  // namespace
}  // namespace chernykh_s_hypercube
