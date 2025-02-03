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

#ifndef GZ_OMNIVERSE_SCENE_HPP
#define GZ_OMNIVERSE_SCENE_HPP

#include "Error.hpp"
#include "ThreadSafe.hpp"

#include <gz/utils/ImplPtr.hh>

#include <gz/math/Pose3.hh>
#include <gz/msgs/joint.pb.h>
#include <gz/msgs/link.pb.h>
#include <gz/msgs/model.pb.h>
#include <gz/msgs/pose.pb.h>
#include <gz/msgs/pose_v.pb.h>
#include <gz/msgs/scene.pb.h>
#include <gz/msgs/vector3d.pb.h>
#include <gz/msgs/visual.pb.h>
#include <gz/transport.hh>

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
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace gz
{
namespace omniverse
{

enum class Simulator : int { gz, IsaacSim };

class Scene
{
 public:
  Scene(
    const std::string &_worldName,
    const std::string &_stageUrl,
    pxr::UsdStageRefPtr stage,
    Simulator _simulatorPoses,
    bool use_live,
    const std::string &_scopeName,
    const gz::math::Pose3d &_scopePose);

  /// \brief Initialize the scene and subscribes for updates. This blocks until
  /// the scene is initialized.
  /// \return true if success
  bool Init();

  /// \brief Equivalent to `scene.Stage().Lock()->Save()`.
  void Save();

  /// Returns a reference to the stage update mutex in this scene.
  std::mutex& Mutex();

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};
}  // namespace omniverse
}  // namespace gz

#endif
