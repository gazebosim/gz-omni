/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEMATERIAL_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEMATERIAL_HH

#include <ignition/rendering.hh>

namespace ignition::rendering::omni
{

  class OmniverseMaterial : public Material
  {
  public:
    bool LightingEnabled() const override;
    void SetLightingEnabled(const bool _enabled) override;
    void SetDepthCheckEnabled(bool _enabled) override;
    bool DepthCheckEnabled() const override;
    void SetDepthWriteEnabled(bool _enabled) override;
    bool DepthWriteEnabled() const override;
    math::Color Ambient() const override;
    void SetAmbient(const double _r, const double _g, const double _b, const double _a = 1.0) override;
    void SetAmbient(const math::Color &_color) override;
    math::Color Diffuse() const override;
    void SetDiffuse(const double _r, const double _g, const double _b, const double _a = 1.0) override;
    void SetDiffuse(const math::Color &_color) override;
    math::Color Specular() const override;
    void SetSpecular(const double _r, const double _g, const double _b, const double _a = 1.0) override;
    void SetSpecular(const math::Color &_color) override;
    math::Color Emissive() const override;
    void SetEmissive(const double _r, const double _g, const double _b, const double _a = 1.0) override;
    void SetEmissive(const math::Color &_color) override;
    double Shininess() const override;
    void SetShininess(const double _shininess) override;
    double Transparency() const override;
    void SetDepthMaterial(const double far, const double near) override;
    void SetTransparency(const double _transparency) override;
    void SetAlphaFromTexture(bool _enabled, double _alpha = 0.5, bool _twoSided = true) override;
    bool TextureAlphaEnabled() const override;
    double AlphaThreshold() const override;
    bool TwoSidedEnabled() const override;
    double Reflectivity() const override;
    void SetReflectivity(const double _reflectivity) override;
    bool CastShadows() const override;
    void SetCastShadows(const bool _castShadows) override;
    bool ReceiveShadows() const override;
    void SetReceiveShadows(const bool _receiveShadows) override;
    bool ReflectionEnabled() const override;
    void SetReflectionEnabled(const bool _enabled) override;
    bool HasTexture() const override;
    std::string Texture() const override;
    void SetTexture(const std::string &_texture) override;
    void ClearTexture() override;
    bool HasNormalMap() const override;
    std::string NormalMap() const override;
    void SetNormalMap(const std::string &_normalMap) override;
    void ClearNormalMap() override;
    bool HasRoughnessMap() const override;
    std::string RoughnessMap() const override;
    void SetRoughnessMap(const std::string &_roughnessMap) override;
    void ClearRoughnessMap() override;
    bool HasMetalnessMap() const override;
    std::string MetalnessMap() const override;
    void SetMetalnessMap(const std::string &_metalnessMap) override;
    void ClearMetalnessMap() override;
    bool HasEnvironmentMap() const override;
    std::string EnvironmentMap() const override;
    void SetEnvironmentMap(const std::string &_metalnessMap) override;
    void ClearEnvironmentMap() override;
    bool HasEmissiveMap() const override;
    std::string EmissiveMap() const override;
    void SetEmissiveMap(const std::string &_emissiveMap) override;
    void ClearEmissiveMap() override;
    bool HasLightMap() const override;
    std::string LightMap() const override;
    unsigned int LightMapTexCoordSet() const override;
    void SetLightMap(const std::string &_lightMap, unsigned int _uvSet = 0u) override;
    void ClearLightMap() override;
    void SetRenderOrder(const float _renderOrder) override;
    float RenderOrder() const override;
    void SetRoughness(const float _roughness) override;
    float Roughness() const override;
    void SetMetalness(const float _metalness) override;
    float Metalness() const override;
    enum MaterialType Type() const override;
    enum ShaderType ShaderType() const override;
    void SetShaderType(enum ShaderType _type) override;
    MaterialPtr Clone(const std::string &_name = "") const override;
    void CopyFrom(ConstMaterialPtr _material) override;
    void CopyFrom(const common::Material &_material) override;
    std::string VertexShader() const override;
    ShaderParamsPtr VertexShaderParams() override;
    void SetVertexShader(const std::string &_path) override;
    std::string FragmentShader() const override;
    ShaderParamsPtr FragmentShaderParams() override;
    void SetFragmentShader(const std::string &_path) override;
  };

} // namespace ignition::rendering::omni

#endif
