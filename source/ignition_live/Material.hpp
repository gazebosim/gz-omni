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
#ifndef IGNITION_OMNIVERSE_MATERIAL_HPP
#define IGNITION_OMNIVERSE_MATERIAL_HPP

#include <ignition/msgs/visual.pb.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdShade/material.h>

namespace ignition
{
namespace omniverse
{
bool SetMaterial(const pxr::UsdGeomGprim& _gprim,
                 const ignition::msgs::Visual& _visualMsg,
                 const pxr::UsdStageRefPtr& _stage);
}
}  // namespace ignition

#endif
