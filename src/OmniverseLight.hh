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
 * distributed under the License is distributed on an "AS IS" BASIS,  * WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 */

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSELIGHT_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSELIGHT_HH

#include <ignition/rendering/base/BaseLight.hh>

#include "OmniverseObject.hh"

namespace ignition::rendering::omni {

class OmniverseLight : public BaseLight<OmniverseObject> {
  void SetDiffuseColor(const math::Color &_color) override;

  void SetSpecularColor(const math::Color &_color) override;

  void SetAttenuationConstant(double _value) override;

  void SetAttenuationLinear(double _value) override;

  void SetAttenuationQuadratic(double _value) override;

  void SetAttenuationRange(double _range) override;

  void SetCastShadows(bool _castShadows) override;
};

class OmniverseDirectionalLight : public BaseDirectionalLight<OmniverseLight> {
 public:
  void SetDirection(const math::Vector3d &_dir) override;
};

class OmniversePointLight : public BasePointLight<OmniverseLight> {};

class OmniverseSpotLight : public BaseSpotLight<OmniverseLight> {
 public:
  void SetDirection(const math::Vector3d &_dir) override;

  void SetInnerAngle(const math::Angle &_angle) override;

  void SetOuterAngle(const math::Angle &_angle) override;

  void SetFalloff(double _falloff) override;
};

}  // namespace ignition::rendering::omni

#endif
