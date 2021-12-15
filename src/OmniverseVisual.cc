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

namespace ignition::rendering::omni {

GeometryStorePtr OmniverseVisual::Geometries() const {
  // TODO: implement
  return nullptr;
}

bool OmniverseVisual::AttachGeometry(GeometryPtr _geometry) {
  // TODO: implement
  return false;
}

bool OmniverseVisual::DetachGeometry(GeometryPtr _geometry) { return false; }

}  // namespace ignition::rendering::omni
