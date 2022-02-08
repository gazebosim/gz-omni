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

#ifndef GET_OP_HPP
#define GET_OP_HPP

#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

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
    this->rotZYX = pxr::GfVec3f(0);
    this->scale = pxr::GfVec3f(1);

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
          rotateOp.Get(&this->rotZYX);
          break;
        case pxr::UsdGeomXformOp::TypeScale:
          scaleOp = xFormOps[i];
          scaleOp.Get(&this->scale);
          break;
      }
    }
  }
  //
  // GetOp(pxr::UsdPrim& _prim)
  // {
  //   this->position = pxr::GfVec3d(0);
  //   this->rotZYX = pxr::GfVec3f(0);
  //   this->scale = pxr::GfVec3f(1);
  //
  //   auto variant_geom = pxr::UsdGeomGprim(_prim);
  //   auto transforms = variant_geom.GetXformOpOrderAttr();
  //
  //   pxr::VtTokenArray xformOpOrder;
  //   transforms.Get(&xformOpOrder);
  //   for (auto & op: xformOpOrder)
  //   {
  //     // std::string s = op;
  //     if (op == "xformOp:scale")
  //     {
  //       auto attribute = _prim.GetAttribute(pxr::TfToken("xformOp:scale"));
  //       if (attribute.GetTypeName().GetCPPTypeName() == "GfVec3f")
  //       {
  //         attribute.Get(&this->scale);
  //       }
  //       else if (attribute.GetTypeName().GetCPPTypeName() == "GfVec3d")
  //       {
  //         pxr::GfVec3d scaleTmp(1, 1, 1);
  //         attribute.Get(&scaleTmp);
  //         this->scale[0] = scaleTmp[0];
  //         this->scale[1] = scaleTmp[1];
  //         this->scale[2] = scaleTmp[2];
  //       }
  //     }
  //     else if (op == "xformOp:rotateZYX")
  //     {
  //       pxr::GfVec3f rotationEuler(0, 0, 0);
  //       auto attribute = _prim.GetAttribute(pxr::TfToken("xformOp:rotateZYX"));
  //       if (attribute.GetTypeName().GetCPPTypeName() == "GfVec3f")
  //       {
  //         attribute.Get(&this->rotZYX);
  //       }
  //       else if (attribute.GetTypeName().GetCPPTypeName() == "GfVec3d")
  //       {
  //         pxr::GfVec3d rotationEulerTmp(0, 0, 0);
  //         attribute.Get(&rotationEulerTmp);
  //         this->rotZYX[0] = rotationEulerTmp[0];
  //         this->rotZYX[1] = rotationEulerTmp[1];
  //         this->rotZYX[2] = rotationEulerTmp[2];
  //       }
  //     }
  //     else if (op == "xformOp:translate")
  //     {
  //       auto attribute = _prim.GetAttribute(pxr::TfToken("xformOp:translate"));
  //       if (attribute.GetTypeName().GetCPPTypeName() == "GfVec3f")
  //       {
  //         attribute.Get(&this->position);
  //       }
  //       else if (attribute.GetTypeName().GetCPPTypeName() == "GfVec3d")
  //       {
  //         pxr::GfVec3d translateTmp(0, 0, 0);
  //         attribute.Get(&translateTmp);
  //         this->position[0] = translateTmp[0];
  //         this->position[1] = translateTmp[1];
  //         this->position[2] = translateTmp[2];
  //       }
  //     }
  //     // else if (op == "xformOp:orient")
  //     // {
  //     //   auto attribute = _prim.GetAttribute(pxr::TfToken("xformOp:orient"));
  //     //   if (attribute.GetTypeName().GetCPPTypeName() == "GfQuatf")
  //     //   {
  //     //     attribute.Get(&rotationQuad);
  //     //   }
  //     //   else if (attribute.GetTypeName().GetCPPTypeName() == "GfQuatd")
  //     //   {
  //     //     pxr::GfQuatd rotationQuadTmp;
  //     //     attribute.Get(&rotationQuadTmp);
  //     //     rotationQuad.SetImaginary(
  //     //       rotationQuadTmp.GetImaginary()[0],
  //     //       rotationQuadTmp.GetImaginary()[1],
  //     //       rotationQuadTmp.GetImaginary()[2]);
  //     //     rotationQuad.SetReal(rotationQuadTmp.GetReal());
  //     //   }
  //     //   ignition::math::Quaterniond q(
  //     //     rotationQuad.GetReal(),
  //     //     rotationQuad.GetImaginary()[0],
  //     //     rotationQuad.GetImaginary()[1],
  //     //     rotationQuad.GetImaginary()[2]);
  //     //   t.q.push_back(q);
  //     //   std::cerr << "rotationQuad " << rotationQuad << '\n';
  //     //   t.isRotation = true;
  //     // }
  //     //
  //     // if (op == "xformOp:transform")
  //     // {
  //     //   // FIXME: Shear is lost (does sdformat support it?).
  //     //
  //     //   pxr::GfMatrix4d transform;
  //     //   _prim.GetAttribute(pxr::TfToken("xformOp:transform")).Get(&transform);
  //     //   const auto rot = transform.RemoveScaleShear();
  //     //   const auto scaleShear = transform * rot.GetInverse();
  //     //
  //     //   t.scale[0] = scaleShear[0][0];
  //     //   t.scale[1] = scaleShear[1][1];
  //     //   t.scale[2] = scaleShear[2][2];
  //     //
  //     //   const auto rotQuat = rot.ExtractRotationQuat();
  //     //   t.translate = ignition::math::Vector3d(transform[3][0], transform[3][1], transform[3][2]);
  //     //   ignition::math::Quaterniond q(
  //     //     rotQuat.GetReal(),
  //     //     rotQuat.GetImaginary()[0],
  //     //     rotQuat.GetImaginary()[1],
  //     //     rotQuat.GetImaginary()[2]
  //     //   );
  //     //   t.q.push_back(q);
  //     //   t.isTranslate = true;
  //     //   t.isRotation = true;
  //     // }
  //   }
  // }

  // Define storage for the different xform ops that Omniverse Kit likes to use
  pxr::UsdGeomXformOp translateOp;
  pxr::UsdGeomXformOp rotateOp;
  pxr::UsdGeomXformOp scaleOp;
  pxr::GfVec3d position;
  pxr::GfVec3f rotZYX;
  pxr::GfVec3f scale;
};
}  // namespace omniverse
}  // namespace ignition

#endif
