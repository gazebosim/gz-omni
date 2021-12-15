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

#include "OmniverseLight.hh"

namespace ignition::rendering::omni {

///////////
// Light //
///////////
void OmniverseLight::SetDiffuseColor(const math::Color &_color) {
  // TODO: implement
}

void OmniverseLight::SetSpecularColor(const math::Color &_color) {
  // TODO: implement
}

void OmniverseLight::SetAttenuationConstant(double _value) {
  // TODO: implement
}

void OmniverseLight::SetAttenuationLinear(double _value) {
  // TODO: implement
}

void OmniverseLight::SetAttenuationQuadratic(double _value) {
  // TODO: implement
}

void OmniverseLight::SetAttenuationRange(double _range) {
  // TODO: implement
}

void OmniverseLight::SetCastShadows(bool _castShadows) {
  // TODO: implement
}

//////////////////////
// DirectionalLight //
//////////////////////
void OmniverseDirectionalLight::SetDirection(const math::Vector3d &_dir) {
  // TODO: implement
}

///////////////
// SpotLight //
///////////////
void OmniverseSpotLight::SetDirection(const math::Vector3d &_dir) {
  // TODO: implement
}

void OmniverseSpotLight::SetInnerAngle(const math::Angle &_angle) {
  // TODO: implement
}

void OmniverseSpotLight::SetOuterAngle(const math::Angle &_angle) {
  // TODO: implement
}

void OmniverseSpotLight::SetFalloff(double _falloff) {
  // TODO: implement
}

}  // namespace ignition::rendering::omni
