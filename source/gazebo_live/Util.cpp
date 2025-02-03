/*
 * Copyright (C) 2025 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "Util.hpp"

#include <gz/common/Util.hh>

namespace gz::omniverse
{
std::string validPath(const std::string &_path)
{
  std::string result;
  if (_path.empty())
  {
    return result;
  }
  result = gz::common::replaceAll(_path, " ", "");
  result = gz::common::replaceAll(result, ".", "_");
  result = gz::common::replaceAll(result, "-", "_");
  result = gz::common::replaceAll(result, "<", "_");
  result = gz::common::replaceAll(result, ">", "_");
  result = gz::common::replaceAll(result, "[", "_");
  result = gz::common::replaceAll(result, "]", "_");
  if (std::isdigit(result[0]))
  {
    result = "_" + result;
  }
  return result;
}
}  // namespace gz::omniverse
