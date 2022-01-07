/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include "OmniverseNode.hh"

#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include "Utils.hh"

namespace ignition::rendering::omni {

void OmniverseNode::InitNode(unsigned int _id, const std::string &_name,
                             OmniverseScene::SharedPtr _scene) {
  this->InitObject(_id, _name, _scene);
  auto prim = this->Stage()->DefinePrim(
      pxr::SdfPath("/_nodes/" + NameToSdfPath(_name)));
  this->gprim = pxr::UsdGeomGprim(prim);
}

math::Vector3d OmniverseNode::LocalScale() const {
  pxr::GfMatrix4d xform;
  bool resetXformStack = false;
  this->gprim.GetLocalTransformation(&xform, &resetXformStack,
                                     pxr::UsdTimeCode::Default());
  return math::Vector3d{xform[0][0], xform[1][1], xform[2][2]};
}

bool OmniverseNode::InheritScale() const {
  return !this->gprim.GetResetXformStack();
}

void OmniverseNode::SetInheritScale(bool _inherit) {
  this->gprim.SetResetXformStack(!_inherit);
}

math::Pose3d OmniverseNode::RawLocalPose() const {
  bool resetXformStack = false;
  pxr::GfMatrix4d xform;
  this->gprim.GetLocalTransformation(&xform, &resetXformStack,
                                     pxr::UsdTimeCode::Default());
  auto pos = xform.ExtractTranslation();
  auto rot = xform.ExtractRotationQuat();
  auto &imag = rot.GetImaginary();
  math::Angle w;
  math::Angle x;
  math::Angle y;
  math::Angle z;
  w.SetDegree(rot.GetReal());
  x.SetDegree(imag[0]);
  y.SetDegree(imag[1]);
  z.SetDegree(imag[2]);
  // TODO: Check correctness
  return math::Pose3d{{pos[0], pos[1], pos[2]},
                      {w.Radian(), x.Radian(), y.Radian(), z.Radian()}};
}

void OmniverseNode::SetRawLocalPose(const math::Pose3d &_pose) {
  pxr::UsdGeomXformCommonAPI xform{this->gprim.GetPrim()};
  xform.SetTranslate({_pose.X(), _pose.Y(), _pose.Z()});
  xform.SetRotate({math::Angle{_pose.Roll()}.Degree(),
                   math::Angle{_pose.Pitch()}.Degree(),
                   math::Angle{_pose.Yaw()}.Degree()});
}

NodeStorePtr OmniverseNode::Children() const { return this->store; }

bool OmniverseNode::AttachChild(NodePtr _child) {
  // TODO: Implement
  auto ovChild = std::dynamic_pointer_cast<OmniverseNode>(_child);
  if (!ovChild) {
    ignerr << "Unable to attach child '" << _child->Name() << "' to parent '"
           << this->Name() << "' (not an omniverse node)" << std::endl;
    return false;
  }
  auto prim = this->Stage()->DefinePrim(this->gprim.GetPath().AppendPath(
      pxr::SdfPath(NameToSdfPath(ovChild->Name()))));
  prim.GetReferences().AddInternalReference(ovChild->gprim.GetPath());
  return true;
}

bool OmniverseNode::DetachChild(NodePtr _child) {
  this->Stage()->RemovePrim(this->gprim.GetPath().AppendPath(
      pxr::SdfPath(NameToSdfPath(_child->Name()))));
  return true;
}

void OmniverseNode::SetLocalScaleImpl(const math::Vector3d &_scale) {
  pxr::UsdGeomXformCommonAPI xform(this->gprim.GetPrim());
  xform.SetScale({_scale.X(), _scale.Y(), _scale.Z()});
  return;
}

}  // namespace ignition::rendering::omni
