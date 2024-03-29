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

#ifndef IGNITION_OMNIVERSE_GET_OP_HPP
#define IGNITION_OMNIVERSE_GET_OP_HPP

#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/quatf.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include <ignition/math/Quaternion.hh>

namespace ignition
{
namespace omniverse
{
// A utility class to get the position, rotation, or scale values
class GetOp
{
 public:
  GetOp(pxr::UsdGeomXformable& xForm)
  {
    this->position = pxr::GfVec3d(0);
    this->rotXYZ = pxr::GfVec3f(0);
    this->scale = pxr::GfVec3f(1);
    ignition::math::Quaterniond orientQuat;

    bool resetXformStack = false;
    std::vector<pxr::UsdGeomXformOp> xFormOps =
        xForm.GetOrderedXformOps(&resetXformStack);

    // Get the current xform op values
    for (size_t i = 0; i < xFormOps.size(); i++)
    {
      switch (xFormOps[i].GetOpType())
      {
        case pxr::UsdGeomXformOp::TypeTranslate:
          translateOp = xFormOps[i];
          translateOp.Get(&this->position);
          break;
        case pxr::UsdGeomXformOp::TypeRotateXYZ:
          rotateOp = xFormOps[i];
          rotateOp.Get(&this->rotXYZ);
          break;
        case pxr::UsdGeomXformOp::TypeOrient:
          rotateOp = xFormOps[i];
          rotateOp.Get(&this->rotQ);
          orientQuat = ignition::math::Quaterniond(
            this->rotQ.GetReal(),
            this->rotQ.GetImaginary()[0],
            this->rotQ.GetImaginary()[1],
            this->rotQ.GetImaginary()[2]);
          this->rotXYZ = pxr::GfVec3f(
            orientQuat.Roll(), orientQuat.Pitch(), orientQuat.Yaw());
          break;
        case pxr::UsdGeomXformOp::TypeScale:
          scaleOp = xFormOps[i];
          scaleOp.Get(&this->scale);
          break;
      }
    }
  }

  // Define storage for the different xform ops that Omniverse Kit likes to use
  pxr::UsdGeomXformOp translateOp;
  pxr::UsdGeomXformOp rotateOp;
  pxr::UsdGeomXformOp scaleOp;
  pxr::GfVec3d position;
  pxr::GfVec3f rotXYZ;
  pxr::GfVec3f scale;
  pxr::GfQuatf rotQ;
};
}  // namespace omniverse
}  // namespace ignition

#endif
