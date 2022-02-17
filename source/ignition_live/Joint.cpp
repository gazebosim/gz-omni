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

#include "Joint.hpp"

pxr::UsdPrim CreateFixedJoint(const std::string& _path,
                              const pxr::UsdStageRefPtr& _stage)
{
  pxr::TfToken usdPrimTypeName("PhysicsFixedJoint");
  return _stage->DefinePrim(pxr::SdfPath(_path), usdPrimTypeName);
}

pxr::UsdPrim CreateRevoluteJoint(const std::string& _path,
                                 const pxr::UsdStageRefPtr& _stage)
{
  pxr::TfToken usdPrimTypeName("PhysicsRevoluteJoint");
  return _stage->DefinePrim(pxr::SdfPath(_path), usdPrimTypeName);
}
