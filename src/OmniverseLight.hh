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

#include <ignition/rendering.hh>

namespace ignition::rendering::omni {

class OmniverseLight : public Light {
 public:
  math::Color DiffuseColor() const override;
  void SetDiffuseColor(double _r, double _g, double _b,
                       double _a = 1.0) override;
  void SetDiffuseColor(const math::Color &_color) override;
  math::Color SpecularColor() const override;
  void SetSpecularColor(double _r, double _g, double _b,
                        double _a = 1.0) override;
  void SetSpecularColor(const math::Color &_color) override;
  double AttenuationConstant() const override;
  void SetAttenuationConstant(double _value) override;
  double AttenuationLinear() const override;
  void SetAttenuationLinear(double _value) override;
  double AttenuationQuadratic() const override;
  void SetAttenuationQuadratic(double _value) override;
  double AttenuationRange() const override;
  void SetAttenuationRange(double _range) override;
  bool CastShadows() const override;
  void SetCastShadows(bool _castShadows) override;
  double Intensity() const override;
  void SetIntensity(double _intensity) override;
};

class OmniverseDirectionalLight : public DirectionalLight {
 public:
  math::Vector3d Direction() const override;
  void SetDirection(double _x, double _y, double _z) override;
  void SetDirection(const math::Vector3d &_dir) override;
};

class OmniversePointLight : public PointLight {};

class OmniverseSpotLight : public SpotLight {
 public:
  math::Vector3d Direction() const override;
  void SetDirection(double _x, double _y, double _z) override;
  void SetDirection(const math::Vector3d &_dir) override;
  math::Angle InnerAngle() const override;
  void SetInnerAngle(double _radians) override;
  void SetInnerAngle(const math::Angle &_angle) override;
  math::Angle OuterAngle() const override;
  void SetOuterAngle(double _radians) override;
  void SetOuterAngle(const math::Angle &_angle) override;
  double Falloff() const override;
  void SetFalloff(double _falloff) override;
};
}  // namespace ignition::rendering::omni

#endif
