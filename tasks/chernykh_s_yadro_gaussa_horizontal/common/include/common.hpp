#pragma once

#include <string>
#include <vector>

#include "task/include/task.hpp"

namespace chernykh_s_yadro_gaussa_horizontal {

using InType = std::tuple<int, int, std::vector<int>>;
using OutType = std::vector<int>;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace chernykh_s_yadro_gaussa_horizontal
