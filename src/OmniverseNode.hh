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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSENODE_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSENODE_HH

#include <ignition/rendering/base/BaseNode.hh>

#include "OmniverseObject.hh"

namespace ignition::rendering::omni {

class OmniverseNode : public BaseNode<OmniverseObject> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseNode>;

  template <typename... Args>
  static OmniverseNode::SharedPtr Make(Args &&...args) {
    return std::shared_ptr<OmniverseNode>(
        new OmniverseNode(std::forward<Args>(args)...));
  }

  NodePtr Parent() const override;

  math::Vector3d LocalScale() const override;

  bool InheritScale() const override;

 protected:
  OmniverseNode() = default;

  math::Pose3d RawLocalPose() const override;

  void SetRawLocalPose(const math::Pose3d &_pose) override;

  NodeStorePtr Children() const override;

  bool AttachChild(NodePtr _child) override;

  bool DetachChild(NodePtr _child) override;

  void SetLocalScaleImpl(const math::Vector3d &_scale) override;
};

}  // namespace ignition::rendering::omni

#endif
