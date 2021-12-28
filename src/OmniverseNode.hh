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

#include <pxr/usd/usd/prim.h>

#include <ignition/rendering/base/BaseNode.hh>
#include <ignition/rendering/base/BaseStorage.hh>

#include "OmniverseObject.hh"
#include "OmniverseScene.hh"

namespace ignition::rendering::omni {

class OmniverseNode : public BaseNode<OmniverseObject> {
 public:
  using Store = BaseNodeStore<OmniverseNode>;
  using StorePtr = std::shared_ptr<Store>;
  using SharedPtr = std::shared_ptr<OmniverseNode>;

  static SharedPtr Make(unsigned int _id, const std::string &_name,
                        OmniverseScene::SharedPtr _scene) {
    auto sp = std::make_shared<OmniverseNode>();
    sp->InitObject(_id, _name, _scene);
    return sp;
  }

  void SetParent(OmniverseNode::SharedPtr _parent) { this->parent = parent; }

  pxr::UsdPrim Prim() const { return this->prim; }

  void SetPrim(pxr::UsdPrim _prim) { this->prim = _prim; }

  bool HasParent() const override { return (bool)this->parent; }

  NodePtr Parent() const override { return this->parent; }

  math::Vector3d LocalScale() const override;

  bool InheritScale() const override;

  void SetInheritScale(bool _inherit) override;

 protected:
  math::Pose3d RawLocalPose() const override;

  void SetRawLocalPose(const math::Pose3d &_pose) override;

  NodeStorePtr Children() const override;

  bool AttachChild(NodePtr _child) override;

  bool DetachChild(NodePtr _child) override;

  void SetLocalScaleImpl(const math::Vector3d &_scale) override;

 private:
  pxr::UsdPrim prim;
  OmniverseNode::SharedPtr parent;
  StorePtr _store = std::make_shared<Store>();
};

using OmniverseNodeStore = OmniverseNode::Store;

}  // namespace ignition::rendering::omni

#endif
