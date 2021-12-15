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

#include <exception>

namespace ignition::rendering::omni {
unsigned int OmniverseVisual::GeometryCount() const {
  throw std::runtime_error("not implemented");
}
bool OmniverseVisual::HasGeometry(ConstGeometryPtr _geometry) const {
  throw std::runtime_error("not implemented");
}
GeometryPtr OmniverseVisual::GeometryByIndex(unsigned int _index) const {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::AddGeometry(GeometryPtr _geometry) {
  throw std::runtime_error("not implemented");
}
GeometryPtr OmniverseVisual::RemoveGeometry(GeometryPtr _geometry) {
  throw std::runtime_error("not implemented");
}
GeometryPtr OmniverseVisual::RemoveGeometryByIndex(unsigned int _index) {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::RemoveGeometries() {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetMaterial(const std::string &_name, bool _unique) {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetMaterial(MaterialPtr _material, bool _unique) {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetChildMaterial(MaterialPtr _material, bool _unique) {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetGeometryMaterial(MaterialPtr _material, bool _unique) {
  throw std::runtime_error("not implemented");
}
MaterialPtr OmniverseVisual::Material() const {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetWireframe(bool _show) {
  throw std::runtime_error("not implemented");
}
bool OmniverseVisual::Wireframe() const {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetVisible(bool _visible) {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::SetVisibilityFlags(uint32_t _flags) {
  throw std::runtime_error("not implemented");
}
uint32_t OmniverseVisual::VisibilityFlags() const {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::AddVisibilityFlags(uint32_t _flags) {
  throw std::runtime_error("not implemented");
}
void OmniverseVisual::RemoveVisibilityFlags(uint32_t _flags) {
  throw std::runtime_error("not implemented");
}
ignition::math::AxisAlignedBox OmniverseVisual::BoundingBox() const {
  throw std::runtime_error("not implemented");
}
ignition::math::AxisAlignedBox OmniverseVisual::LocalBoundingBox() const {
  throw std::runtime_error("not implemented");
}
VisualPtr OmniverseVisual::Clone(const std::string &_name,
                                 NodePtr _newParent) const {
  throw std::runtime_error("not implemented");
}
}  // namespace ignition::rendering::omni
