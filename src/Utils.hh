/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#ifndef IGNITION_RENDERING_OMNI_UTILS_HH
#define IGNITION_RENDERING_OMNI_UTILS_HH

#include <algorithm>
#include <string>

namespace ignition::rendering::omni {

/// \brief Convert a object name so that it is suitable as a sdf path.
inline std::string NameToSdfPath(const std::string& _object_name) {
  // example ign name "test_scene::Visual(65535)"
  std::string result = _object_name.substr(_object_name.find_last_of(':') +
                                           1);  // remove the scene namespace
  std::replace(result.begin(), result.end(), '(', '_');
  std::replace(result.begin(), result.end(), ')', '_');
  std::replace(result.begin(), result.end(), '/', '_');
  return result;
}

}  // namespace ignition::rendering::omni

#endif
