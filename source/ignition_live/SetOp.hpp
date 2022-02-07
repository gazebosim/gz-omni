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

#ifndef SET_OP_HPP
#define SET_OP_HPP

#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/usdGeom/xform.h>

namespace ignition
{
namespace omniverse
{
// A utility class to set the position, rotation, or scale values
class SetOp
{
 public:
  SetOp(pxr::UsdGeomXformable& xForm, pxr::UsdGeomXformOp& op,
        pxr::UsdGeomXformOp::Type opType, const pxr::GfVec3d& value,
        const pxr::UsdGeomXformOp::Precision precision)
  {
    if (!op)
    {
      op = xForm.AddXformOp(opType, precision);
    }

    if (op.GetPrecision() == pxr::UsdGeomXformOp::Precision::PrecisionFloat)
      op.Set(pxr::GfVec3f(value));
    else
      op.Set(value);
  }
};
}  // namespace omniverse
}  // namespace ignition

#endif
