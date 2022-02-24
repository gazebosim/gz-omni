/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IGNITION_OMNIVERSE_SCENE_HPP
#define IGNITION_OMNIVERSE_SCENE_HPP

#include "Error.hpp"
#include "ThreadSafe.hpp"

#include <ignition/utils/ImplPtr.hh>

#include <ignition/math/Pose3.hh>
#include <ignition/msgs/joint.pb.h>
#include <ignition/msgs/link.pb.h>
#include <ignition/msgs/model.pb.h>
#include <ignition/msgs/pose.pb.h>
#include <ignition/msgs/pose_v.pb.h>
#include <ignition/msgs/scene.pb.h>
#include <ignition/msgs/vector3d.pb.h>
#include <ignition/msgs/visual.pb.h>
#include <ignition/transport.hh>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace ignition
{
namespace omniverse
{
class Scene
{
 public:
  Scene(
    const std::string &_worldName,
    const std::string &_stageUrl,
    const std::string &_simulatorPoses);

  /// \brief Initialize the scene and subscribes for updates. This blocks until
  /// the scene is initialized.
  /// \return true if success
  bool Init();

  /// \brief Equivalent to `scene.Stage().Lock()->Save()`.
  void Save();

  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> &Stage();

  /// \internal
  /// \brief Private data pointer
  IGN_UTILS_IMPL_PTR(dataPtr)
};
}  // namespace omniverse
}  // namespace ignition

#endif
