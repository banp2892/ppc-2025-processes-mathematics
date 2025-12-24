#include <gtest/gtest.h>
#include <mpi.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
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
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }
  InType GetTestInputData() override {
    return input_data_;
  }

  void ReadImage(const char *filename, int crop_size) {
    int x = 0;
    int y = 0;
    int channels = 0;
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_chernykh_s_yadro_gaussa_horizontal, filename);
    unsigned char *data = stbi_load(abs_path.c_str(), &x, &y, &channels, STBI_grey);
    if (data == nullptr) {
      throw std::runtime_error("Failed to load: " + abs_path);
    }
    int actual_w = std::min(x, crop_size);
    int actual_h = std::min(y, crop_size);
    std::vector<int> pixels;
    pixels.reserve(static_cast<size_t>(actual_w) * static_cast<size_t>(actual_h));
    for (int i = 0; i < actual_h; i++) {
      for (int j = 0; j < actual_w; j++) {
        int pixel_value = static_cast<int>(data[(i * x) + j]);
        pixels.push_back(pixel_value);
      }
    }

    input_data_ = std::make_tuple(actual_w, actual_h, pixels);
    stbi_image_free(data);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int size = std::get<0>(params);
    ReadImage("chigur.jpg", size);
  }
  bool CheckTestOutputData(OutType &output_data) final {
  int rank = 0;
  int is_mpi_active = 0;
  MPI_Initialized(&is_mpi_active);
  if (is_mpi_active != 0) {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  }

  if (rank != 0) {
    return true;
  }
  ChernykhSYadroGaussaHorizontalSEQ task_seq(input_data_);
  if (task_seq.Validation()) {
    task_seq.PreProcessing();
    task_seq.Run();
    task_seq.PostProcessing();
  } else {
    return false;
  }
  return output_data == task_seq.GetOutput();
}

 private:
  InType input_data_;
};
namespace {

TEST_P(ChernykhSRunFuncTestsGaussaHorizontal, YadroGaussa) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 5> kTestParam = {
    std::make_tuple(128, "chigur_128x128"), std::make_tuple(256, "chigur_256x256"),
    std::make_tuple(512, "chigur_512x512"), std::make_tuple(800, "chigur_800x800"),
    std::make_tuple(1024, "chigur_1024x1024")};
const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ChernykhSYadroGaussaHorizontalMPI, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal),
                                           ppc::util::AddFuncTask<ChernykhSYadroGaussaHorizontalSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal));

TEST(ChernykhSYadroGaussaHorizontalSEQManual, Test3x3FullMatrixCheck) {
  int w = 3;
  int h = 3;
  std::vector<int> p = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  InType input_data = std::make_tuple(w, h, p);
  ChernykhSYadroGaussaHorizontalSEQ test_task(input_data);

  ASSERT_TRUE(test_task.Validation());
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  std::vector<int> out = test_task.GetOutput();
  ASSERT_EQ(out.size(), 9U);
  const int c = 4;
  const int e = 2;
  const int k = 1;
  const int s = 16;
  EXPECT_EQ(out[0],
            (p[0] * k + p[0] * e + p[1] * k + p[0] * e + p[0] * c + p[1] * e + p[3] * k + p[3] * e + p[4] * k) / s);
  EXPECT_EQ(out[1],
            (p[0] * k + p[1] * e + p[2] * k + p[0] * e + p[1] * c + p[2] * e + p[3] * k + p[4] * e + p[5] * k) / s);
  EXPECT_EQ(out[2],
            (p[1] * k + p[2] * e + p[2] * k + p[1] * e + p[2] * c + p[2] * e + p[4] * k + p[5] * e + p[5] * k) / s);
  EXPECT_EQ(out[3],
            (p[0] * k + p[0] * e + p[1] * k + p[3] * e + p[3] * c + p[4] * e + p[6] * k + p[6] * e + p[7] * k) / s);
  EXPECT_EQ(out[4],
            (p[0] * k + p[1] * e + p[2] * k + p[3] * e + p[4] * c + p[5] * e + p[6] * k + p[7] * e + p[8] * k) / s);
  EXPECT_EQ(out[5],
            (p[1] * k + p[2] * e + p[2] * k + p[4] * e + p[5] * c + p[5] * e + p[7] * k + p[8] * e + p[8] * k) / s);
  EXPECT_EQ(out[6],
            (p[3] * k + p[3] * e + p[4] * k + p[6] * e + p[6] * c + p[7] * e + p[6] * k + p[6] * e + p[7] * k) / s);
  EXPECT_EQ(out[7],
            (p[3] * k + p[4] * e + p[5] * k + p[6] * e + p[7] * c + p[8] * e + p[6] * k + p[7] * e + p[8] * k) / s);
  EXPECT_EQ(out[8],
            (p[4] * k + p[5] * e + p[5] * k + p[7] * e + p[8] * c + p[8] * e + p[7] * k + p[8] * e + p[8] * k) / s);
}

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    ChernykhSRunFuncTestsGaussaHorizontal::PrintFuncTestName<ChernykhSRunFuncTestsGaussaHorizontal>;

INSTANTIATE_TEST_SUITE_P(YadroGaussaHorizontalTest, ChernykhSRunFuncTestsGaussaHorizontal, kGtestValues, kPerfTestName);

}  // namespace
}  // namespace chernykh_s_yadro_gaussa_horizontal
