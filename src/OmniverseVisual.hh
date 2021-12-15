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

#include <ignition/rendering.hh>

namespace ignition::rendering::omni {

class OmniverseVisual : public Visual {
 public:
  unsigned int GeometryCount() const override;
  bool HasGeometry(ConstGeometryPtr _geometry) const override;
  GeometryPtr GeometryByIndex(unsigned int _index) const override;
  void AddGeometry(GeometryPtr _geometry) override;
  GeometryPtr RemoveGeometry(GeometryPtr _geometry) override;
  GeometryPtr RemoveGeometryByIndex(unsigned int _index) override;
  void RemoveGeometries() override;
  void SetMaterial(const std::string &_name, bool _unique = true) override;
  void SetMaterial(MaterialPtr _material, bool _unique = true) override;
  void SetChildMaterial(MaterialPtr _material, bool _unique = true) override;
  void SetGeometryMaterial(MaterialPtr _material, bool _unique = true) override;
  MaterialPtr Material() const override;
  void SetWireframe(bool _show) override;
  bool Wireframe() const override;
  void SetVisible(bool _visible) override;
  void SetVisibilityFlags(uint32_t _flags) override;
  uint32_t VisibilityFlags() const override;
  void AddVisibilityFlags(uint32_t _flags) override;
  void RemoveVisibilityFlags(uint32_t _flags) override;
  ignition::math::AxisAlignedBox BoundingBox() const override;
  ignition::math::AxisAlignedBox LocalBoundingBox() const override;
  VisualPtr Clone(const std::string &_name, NodePtr _newParent) const override;
};

}  // namespace ignition::rendering::omni

#endif
