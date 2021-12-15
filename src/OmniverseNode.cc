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

namespace ignition::rendering::omni {

NodePtr OmniverseNode::Parent() const {
  // TODO: implement
  return nullptr;
}

math::Vector3d OmniverseNode::LocalScale() const {
  // TODO: implement
  return math::Vector3d::One;
}

bool OmniverseNode::InheritScale() const {
  // TODO: implement
  return false;
}

math::Pose3d OmniverseNode::RawLocalPose() const {
  // TODO: implement
  return math::Pose3d::Zero;
}

void OmniverseNode::SetRawLocalPose(const math::Pose3d &_pose) {
  // TODO: implement
}

NodeStorePtr OmniverseNode::Children() const {
  // TODO: implement
  return nullptr;
}

bool OmniverseNode::AttachChild(NodePtr _child) {
  // TODO: implement
  return false;
}

bool OmniverseNode::DetachChild(NodePtr _child) {
  // TODO: implement
  return false;
}

void OmniverseNode::SetLocalScaleImpl(const math::Vector3d &_scale) {
  // TODO: implement
}

}  // namespace ignition::rendering::omni
