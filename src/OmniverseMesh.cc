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

#include "OmniverseMesh.hh"

namespace ignition::rendering::omni {

OmniverseMesh::SharedPtr OmniverseMesh::Make(unsigned int _id,
                                             const std::string& _name,
                                             OmniverseScene::SharedPtr _scene,
                                             const MeshDescriptor& _desc) {
  auto sp = std::shared_ptr<OmniverseMesh>(new OmniverseMesh());
  sp->InitObject(_id, _name, _scene);
  // TODO: create usdmesh
  return sp;
}

SubMeshStorePtr OmniverseMesh::SubMeshes() const {
  // TODO: implement
  return nullptr;
}

void OmniverseSubMesh::SetMaterialImpl(MaterialPtr _material) {
  // TODO: implement
}

}  // namespace ignition::rendering::omni
