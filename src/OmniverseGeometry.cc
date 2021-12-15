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

#include "OmniverseGeometry.hh"

namespace ignition::rendering::omni {

VisualPtr OmniverseGeometry::Parent() const {
  // TODO: implement
  return nullptr;
}

void OmniverseGeometry::SetMaterial(MaterialPtr _material, bool _unique) {
  // TODO: implement
}

bool OmniverseGeometry::HasParent() const {
  return this->_gprim.GetPrim().GetParent().IsValid();
}

MaterialPtr OmniverseGeometry::Material() const {
  // TODO: implement
  return nullptr;
}

OmniverseGeometry::OmniverseGeometry(ScenePtr _scene, pxr::UsdGeomGprim _gprim)
    : OmniverseObject(_scene), _gprim(std::move(_gprim)) {}

}  // namespace ignition::rendering::omni
