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

// FIXME: BaseMesh.hh is missing Console.hh include
#include <ignition/common/Console.hh>
#include <ignition/rendering/base/BaseMesh.hh>

#include "OmniverseObject.hh"

namespace ignition::rendering::omni {

class OmniverseMesh : public BaseMesh<OmniverseObject> {
 public:
  SubMeshStorePtr SubMeshes() const override;
};

class OmniverseSubMesh : public BaseSubMesh<OmniverseObject> {
 protected:
  void SetMaterialImpl(MaterialPtr _material) override;
};

}  // namespace ignition::rendering::omni

#endif
