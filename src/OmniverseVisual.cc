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

#include "OmniverseVisual.hh"

#include <ignition/rendering/base/BaseStorage.hh>

#include "OmniverseGeometry.hh"
#include "Utils.hh"

namespace ignition::rendering::omni {

OmniverseVisual::SharedPtr OmniverseVisual::Make(
    unsigned int _id, const std::string& _name,
    OmniverseScene::SharedPtr _scene) {
  auto sp = std::shared_ptr<OmniverseVisual>(new OmniverseVisual());
  sp->Init(_id, _name, _scene);
  sp->geomStore = std::make_shared<BaseGeometryStore<OmniverseGeometry>>();
  return sp;
}

bool OmniverseVisual::AttachGeometry(GeometryPtr _geometry) {
  auto ovGeom = std::dynamic_pointer_cast<OmniverseGeometry>(_geometry);
  if (!ovGeom) {
    ignerr << "Failed to attach geometry '" << _geometry->Id()
           << "' (not an omniverse geometry)" << std::endl;
    return false;
  }
  return ovGeom->AttachToVisual(
      std::dynamic_pointer_cast<OmniverseVisual>(this->shared_from_this()));
}

bool OmniverseVisual::DetachGeometry(GeometryPtr _geometry) {
  auto ovGeom = std::dynamic_pointer_cast<OmniverseGeometry>(_geometry);
  if (!ovGeom) {
    ignerr << "Failed to detach geometry '" << _geometry->Id()
           << "' (not an omniverse geometry)" << std::endl;
    return false;
  }
  auto path = ovGeom->Gprim().GetPath();
  // NOTE: This does not guarantee the prim is removed due to usd's complex
  // composing system, see the docs for `RemovePrim` for more info.
  this->Stage()->RemovePrim(path);
  return true;
}

}  // namespace ignition::rendering::omni
