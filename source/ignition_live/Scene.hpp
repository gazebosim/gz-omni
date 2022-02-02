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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "IgnitionModel.hpp"

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include "pxr/usd/usd/stage.h"
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdGeom/xform.h>

#include <ignition/msgs/model.pb.h>
#include <ignition/msgs/pose.pb.h>
#include <ignition/msgs/pose_v.pb.h>

#include <ignition/math/Pose3.hh>
#include <ignition/transport.hh>

namespace ignition
{
namespace omniverse
{
class Scene : public std::enable_shared_from_this<Scene>
{
 public:

   using SharedPtr = std::shared_ptr<Scene>;

   virtual std::unordered_map<std::string, IgnitionModel> GetModels() = 0;
   virtual bool SetModelPose(
     const std::string &_name,
     const ignition::math::Pose3d &_pose) = 0;

   virtual bool RemoveModel(const std::string &_name) = 0;

   virtual pxr::UsdPrim GetPrimAtPath(const std::string &_path) = 0;
   virtual void SaveStage() = 0;

   virtual pxr::UsdGeomCapsule createCapsule(const std::string &_name) = 0;
   virtual pxr::UsdGeomSphere createSphere(const std::string &_name) = 0;
   virtual pxr::UsdGeomCube createCube(const std::string &_name) = 0;
   virtual pxr::UsdGeomSphere createEllipsoid(const std::string &_name) = 0;
   virtual pxr::UsdGeomCylinder createCylinder(const std::string &_name) = 0;
   virtual pxr::UsdGeomXform createXform(const std::string &_name) = 0;
   virtual pxr::UsdShadeMaterial createMaterial(const std::string &_name) = 0;
   virtual pxr::UsdShadeShader createShader(const std::string &_name) = 0;

   // virtual pxr::UsdStageRefPtr Stage() const = 0;
protected:
  Scene() = default;
};
}
}

#endif
