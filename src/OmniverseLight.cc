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

#include <exception>

#include "OmniverseLight.hh"

namespace ignition::rendering::omni
{
  math::Color OmniverseLight::DiffuseColor() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetDiffuseColor(double _r, double _g, double _b, double _a = 1.0) { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetDiffuseColor(const math::Color &_color) { throw std::runtime_error("not implemented"); }
  math::Color OmniverseLight::SpecularColor() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetSpecularColor(double _r, double _g, double _b, double _a = 1.0) { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetSpecularColor(const math::Color &_color) { throw std::runtime_error("not implemented"); }
  double OmniverseLight::AttenuationConstant() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetAttenuationConstant(double _value) { throw std::runtime_error("not implemented"); }
  double OmniverseLight::AttenuationLinear() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetAttenuationLinear(double _value) { throw std::runtime_error("not implemented"); }
  double OmniverseLight::AttenuationQuadratic() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetAttenuationQuadratic(double _value) { throw std::runtime_error("not implemented"); }
  double OmniverseLight::AttenuationRange() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetAttenuationRange(double _range) { throw std::runtime_error("not implemented"); }
  bool OmniverseLight::CastShadows() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetCastShadows(bool _castShadows) { throw std::runtime_error("not implemented"); }
  double OmniverseLight::Intensity() const { throw std::runtime_error("not implemented"); }
  void OmniverseLight::SetIntensity(double _intensity) { throw std::runtime_error("not implemented"); }

  math::Vector3d OmniverseDirectionalLight::Direction() const { throw std::runtime_error("not implemented"); }
  void OmniverseDirectionalLight::SetDirection(double _x, double _y, double _z) { throw std::runtime_error("not implemented"); }
  void OmniverseDirectionalLight::SetDirection(const math::Vector3d &_dir) { throw std::runtime_error("not implemented"); }

  math::Vector3d OmniverseSpotLight::Direction() const { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetDirection(double _x, double _y, double _z) { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetDirection(const math::Vector3d &_dir) { throw std::runtime_error("not implemented"); }
  math::Angle OmniverseSpotLight::InnerAngle() const { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetInnerAngle(double _radians) { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetInnerAngle(const math::Angle &_angle) { throw std::runtime_error("not implemented"); }
  math::Angle OmniverseSpotLight::OuterAngle() const { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetOuterAngle(double _radians) { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetOuterAngle(const math::Angle &_angle) { throw std::runtime_error("not implemented"); }
  double OmniverseSpotLight::Falloff() const { throw std::runtime_error("not implemented"); }
  void OmniverseSpotLight::SetFalloff(double _falloff) { throw std::runtime_error("not implemented"); }
} // namespace ignition::rendering::omni
