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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSECAMERA_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSECAMERA_HH

#include <pxr/usd/usdGeom/gprim.h>

#include <ignition/rendering/base/BaseGeometry.hh>

#include "OmniverseObject.hh"

namespace ignition::rendering::omni {

class OmniverseGeometry : public BaseGeometry<OmniverseObject> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseGeometry>;

  template <typename... Args>
  static OmniverseGeometry::SharedPtr Make(Args&&... args) {
    return std::shared_ptr<OmniverseGeometry>(
        new OmniverseGeometry(std::forward<Args>(args)...));
  }

  VisualPtr Parent() const override;

  void SetMaterial(MaterialPtr _material, bool _unique = true) override;

  bool HasParent() const override;

  MaterialPtr Material() const override;

 protected:
  OmniverseGeometry() = default;
  OmniverseGeometry(ScenePtr _scene, pxr::UsdGeomGprim _gprim);

 private:
  pxr::UsdGeomGprim _gprim;
};

}  // namespace ignition::rendering::omni

#endif
