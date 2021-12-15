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

#include <ignition/rendering/base/BaseCamera.hh>

#include "OmniverseSensor.hh"

namespace ignition::rendering::omni {

class OmniverseCamera : public BaseCamera<OmniverseSensor> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseCamera>;

  template <typename... Args>
  static SharedPtr Make(Args&&... args) {
    return std::shared_ptr<OmniverseCamera>(
        new OmniverseCamera(std::forward<Args>(args)...));
  }

  void Render() override;

 protected:
  RenderTargetPtr RenderTarget() const override;
};

}  // namespace ignition::rendering::omni

#endif
