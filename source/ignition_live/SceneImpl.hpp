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

#ifndef IGNITION_OMNIVERSE_SCENEIMPL_HPP
#define IGNITION_OMNIVERSE_SCENEIMPL_HPP

#include "IgnitionModel.hpp"
#include "Scene.hpp"

#include <ignition/math/Pose3.hh>
#include <ignition/msgs/model.pb.h>
#include <ignition/msgs/pose.pb.h>
#include <ignition/msgs/pose_v.pb.h>
#include <ignition/transport.hh>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdGeom/xform.h>

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace ignition
{
namespace omniverse
{
class SceneImpl : public Scene
{
 public:
  SceneImpl(const std::string &_worldName, pxr::UsdStageRefPtr _stage);

  using SharedPtr = std::shared_ptr<Scene>;

  static SharedPtr Make(const std::string &_worldName,
                        pxr::UsdStageRefPtr _stage);

  SharedPtr SharedFromThis()
  {
    return std::static_pointer_cast<Scene>(this->shared_from_this());
  }

  bool Init();
  void CallbackJoint(const ignition::msgs::Model &_msg);
  void CallbackPoses(const ignition::msgs::Pose_V &_msg);
  void modelWorker();

  std::unordered_map<std::string, IgnitionModel> GetModels() override;
  bool SetModelPose(const std::string &_name,
                    const ignition::math::Pose3d &_pose) override;

  bool RemoveModel(const std::string &_name) override;

  // pxr::UsdStageRefPtr Stage() const override;

  pxr::UsdPrim GetPrimAtPath(const std::string &_path) override;
  void SaveStage() override;

  pxr::UsdGeomCapsule CreateCapsule(const std::string &_name) override;
  pxr::UsdGeomSphere CreateSphere(const std::string &_name) override;
  pxr::UsdGeomCube CreateCube(const std::string &_name) override;
  pxr::UsdGeomSphere CreateEllipsoid(const std::string &_name) override;
  pxr::UsdGeomCylinder CreateCylinder(const std::string &_name) override;
  pxr::UsdShadeMaterial CreateMaterial(const std::string &_name) override;
  pxr::UsdShadeShader CreateShader(const std::string &_name) override;
  pxr::UsdGeomXform CreateXform(const std::string &_name) override;

 private:
  std::unordered_map<std::string, IgnitionModel> models;
  std::mutex mutexStage;
  pxr::UsdStageRefPtr stage;
  std::mutex poseMutex;
  std::string worldName;
  std::shared_ptr<std::thread> modelThread;
  ignition::transport::Node node;
};
}  // namespace omniverse
}  // namespace ignition

#endif
