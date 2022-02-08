/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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
#ifndef OMNIVERSE_IGNITION_MODEL_HPP
#define OMNIVERSE_IGNITION_MODEL_HPP

#include "IgnitionJoint.hpp"
// #include "IgnitionVisual.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace ignition
{
namespace omniverse
{
class IgnitionModel
{
 public:
  IgnitionModel() = default;

  ignition::math::Pose3d pose;
  uint id;
  std::string name;
  std::unordered_map<std::string, std::shared_ptr<IgnitionJoint>>
      ignitionJoints;
  // std::vector<IgnitionVisual::SharedPtr> visuals;
};
}  // namespace omniverse
}  // namespace ignition
// std::vector<std::shared_ptr<IgnitionVisual>> visuals;

#endif
