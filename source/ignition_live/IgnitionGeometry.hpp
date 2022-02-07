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
#ifndef OMNIVERSE_IGNITION_GEOMETRY_HPP
#define OMNIVERSE_IGNITION_GEOMETRY_HPP

#include <memory>

#include <ignition/msgs/geometry.pb.h>
#include <ignition/msgs/visual.pb.h>

#include <ignition/math/Pose3.hh>

#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "Scene.hpp"

namespace ignition
{
namespace omniverse
{
class IgnitionGeometry
{
 public:
  using SharedPtr = std::shared_ptr<IgnitionGeometry>;

  static SharedPtr Make(unsigned int _id, const std::string &_name,
                        Scene::SharedPtr &_scene,
                        const ignition::msgs::Geometry::Type &_type);

  static std::string GeometryTypeToString(
      const ignition::msgs::Geometry::Type &_type);

  bool AttachToVisual(const ignition::msgs::Geometry &_geom,
                      const std::string &_path,
                      const pxr::UsdShadeMaterial &_material);

  ignition::msgs::Geometry::Type Type() const;

  uint id;
  std::string name;
  ignition::msgs::Geometry::Type type;
  pxr::UsdGeomGprim gprim;
  Scene::SharedPtr scene;
};
}  // namespace omniverse
}  // namespace ignition

#endif
