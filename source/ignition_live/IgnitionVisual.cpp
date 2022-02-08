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

#include "IgnitionVisual.hpp"
#include "Material.hpp"

#include <pxr/usd/usdGeom/xform.h>

namespace ignition
{
namespace omniverse
{
IgnitionVisual::SharedPtr IgnitionVisual::Make(unsigned int _id,
                                               const std::string &_name,
                                               Scene::SharedPtr &_scene)
{
  auto sp = std::shared_ptr<IgnitionVisual>(new IgnitionVisual());
  sp->id = _id;
  sp->name = _name;
  sp->scene = _scene;
  return sp;
}

std::vector<IgnitionGeometry::SharedPtr> IgnitionVisual::Geometries() const
{
  return this->geomStore;
}

bool IgnitionVisual::AttachGeometry(const ignition::msgs::Visual &_visual,
                                    const std::string &_path)
{
  std::string sdfVisualPath = _path + "/" + _visual.name();
  auto usdVisualXform = this->scene->CreateXform(sdfVisualPath);

  IgnitionGeometry::SharedPtr ignitionGeometry = IgnitionGeometry::Make(
      _visual.id(), _visual.name(), this->scene, _visual.geometry().type());

  auto material = ignition::omniverse::ParseMaterial(_visual, this->scene);

  bool result = ignitionGeometry->AttachToVisual(_visual.geometry(), sdfVisualPath,
                                                 material);

  usdVisualXform.AddTranslateOp(pxr::UsdGeomXformOp::Precision::PrecisionDouble)
    .Set(
      pxr::GfVec3d(
        _visual.pose().position().x(),
        _visual.pose().position().y(),
        _visual.pose().position().z()));

  ignition::math::Quaterniond q(
    _visual.pose().orientation().w(),
    _visual.pose().orientation().x(),
    _visual.pose().orientation().y(),
    _visual.pose().orientation().z());

   usdVisualXform.AddRotateXYZOp(pxr::UsdGeomXformOp::Precision::PrecisionDouble)
    .Set(
       pxr::GfVec3d(
         q.Roll() * 180 /3.1416,
         q.Pitch() * 180 /3.1416,
         q.Yaw() * 180 /3.1416));

  return result;
}

bool IgnitionVisual::DetachGeometry(const ignition::msgs::Visual &_visual,
                                    const std::string &_path)
{
  // this->scene->Stage()->RemovePrim(pxr::SdfPath(_path));
  return true;
}
}  // namespace omniverse
}  // namespace ignition
