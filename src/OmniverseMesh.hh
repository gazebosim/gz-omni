/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEMESH_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEMESH_HH

#include <pxr/usd/usdGeom/mesh.h>

#include <ignition/common/Console.hh>  // FIXTHEM: BaseMesh.hh is missing Console.hh include
#include <ignition/rendering/base/BaseMesh.hh>
#include <optional>

#include "OmniverseObject.hh"
#include "OmniverseScene.hh"

namespace ignition::rendering::omni {

class OmniverseMesh : public BaseMesh<OmniverseObject> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseMesh>;

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        OmniverseScene::SharedPtr _scene,
                        const MeshDescriptor& _desc) {
    auto sp = std::shared_ptr<OmniverseMesh>(new OmniverseMesh());
    sp->InitObject(_id, _name, _scene);
    // TODO: create usdmesh
    return sp;
  }

  bool HasParent() const override;

  VisualPtr Parent() const override;

  void RemoveParent() override;

  SubMeshStorePtr SubMeshes() const override;

 private:
  std::optional<pxr::UsdGeomMesh> usdMesh;
};

class OmniverseSubMesh : public BaseSubMesh<OmniverseObject> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseSubMesh>;

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        OmniverseScene::SharedPtr _scene) {
    auto sp = std::shared_ptr<OmniverseSubMesh>(new OmniverseSubMesh());
    sp->InitObject(_id, _name, _scene);
    return sp;
  }

 protected:
  void SetMaterialImpl(MaterialPtr _material) override;
};

}  // namespace ignition::rendering::omni

#endif
