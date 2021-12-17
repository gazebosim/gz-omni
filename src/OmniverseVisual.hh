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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEVISUAL_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEVISUAL_HH

#include <pxr/usd/usd/stage.h>

#include <ignition/rendering/base/BaseStorage.hh>
#include <ignition/rendering/base/BaseVisual.hh>

#include "OmniverseGeometry.hh"
#include "OmniverseNode.hh"
#include "OmniverseScene.hh"
#include "Utils.hh"

namespace ignition::rendering::omni {

class OmniverseVisual : public BaseVisual<OmniverseNode> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseVisual>;

  static OmniverseVisual::SharedPtr Make(
      unsigned int _id, const std::string& _name,
      OmniverseScene::SharedPtr _scene, OmniverseNode::SharedPtr _parent = {}) {
    auto sp = std::shared_ptr<OmniverseVisual>(new OmniverseVisual());
    sp->InitObject(_id, _name, _scene);
    sp->SetParent(_parent);
    std::string parentPath = "";
    if (_parent) {
      parentPath = _parent->Prim().GetPath().GetString();
    }
    sp->prim = _scene->Stage()->DefinePrim(
        pxr::SdfPath(parentPath + "/" + NameToSdfPath(_name)));
    assert(sp->prim);
    return sp;
  }

 protected:
  inline GeometryStorePtr Geometries() const { return this->_geomStore; }

  bool AttachGeometry(GeometryPtr _geometry) override;

  bool DetachGeometry(GeometryPtr _geometry) override;

 private:
  pxr::UsdPrim prim;
  OmniverseGeometry::StorePtr _geomStore =
      std::make_shared<OmniverseGeometry::Store>();
};

using OmniverseVisualStore = BaseVisualStore<OmniverseVisual>;

}  // namespace ignition::rendering::omni

#endif
