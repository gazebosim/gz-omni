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

#include <ignition/rendering/base/BaseVisual.hh>

#include "OmniverseNode.hh"

namespace ignition::rendering::omni {

class OmniverseVisual : public BaseVisual<OmniverseNode> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseVisual>;

  template <typename... Args>
  static OmniverseVisual::SharedPtr Make(Args&&... args) {
    return std::shared_ptr<OmniverseVisual>(
        new OmniverseVisual(std::forward<Args>(args)...));
  }

 protected:
  GeometryStorePtr Geometries() const override;

  bool AttachGeometry(GeometryPtr _geometry) override;

  bool DetachGeometry(GeometryPtr _geometry) override;
};

}  // namespace ignition::rendering::omni

#endif
