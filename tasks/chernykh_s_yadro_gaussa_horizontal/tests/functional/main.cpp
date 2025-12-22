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
        int pixel_value = static_cast<int>(data[i * x + j]);
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
    int w = std::get<0>(input_data_);
    int h = std::get<1>(input_data_);
    const std::vector<int> &input_pixels = std::get<2>(input_data_);
    std::vector<int> reference_output(static_cast<size_t>(w) * static_cast<size_t>(h));
    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        int sum = 0;
        for (int ki = -1; ki <= 1; ++ki) {
          for (int kj = -1; kj <= 1; ++kj) {
            int stroka = std::clamp(i + ki, 0, h - 1);
            int stolbec = std::clamp(j + kj, 0, w - 1);
            int weight = 0;
            if (ki == 0 && kj == 0) {
              weight = 4;
            } else if (ki == 0 || kj == 0) {
              weight = 2;
            } else {
              weight = 1;
            }
            sum += input_pixels[stroka * w + stolbec] * weight;
          }
        }
        reference_output[i * w + j] = sum / 16;
      }
    }

    return output_data == reference_output;
  }

 private:
  InType input_data_;
};
namespace {

TEST_P(ChernykhSRunFuncTestsGaussaHorizontal, SumHypercube) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 5> kTestParam = {
    std::make_tuple(128, "chigur_128x128"), std::make_tuple(256, "chigur_256x256"),
    std::make_tuple(512, "chigur_512x512"), std::make_tuple(800, "chigur_large_rect"),
    std::make_tuple(1024, "chigur_full_or_max")};
const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ChernykhSYadroGaussaHorizontalMPI, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal),
                                           ppc::util::AddFuncTask<ChernykhSYadroGaussaHorizontalSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_yadro_gaussa_horizontal));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    ChernykhSRunFuncTestsGaussaHorizontal::PrintFuncTestName<ChernykhSRunFuncTestsGaussaHorizontal>;

INSTANTIATE_TEST_SUITE_P(HypercubeTests, ChernykhSRunFuncTestsGaussaHorizontal, kGtestValues, kPerfTestName);

}  // namespace
}  // namespace chernykh_s_yadro_gaussa_horizontal
