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

#include "OmniverseMaterial.hh"

#include <exception>

namespace ignition::rendering::omni {
bool OmniverseMaterial::LightingEnabled() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetLightingEnabled(const bool _enabled) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetDepthCheckEnabled(bool _enabled) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::DepthCheckEnabled() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetDepthWriteEnabled(bool _enabled) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::DepthWriteEnabled() const {
  throw std::runtime_error("not implemented");
}
math::Color OmniverseMaterial::Ambient() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetAmbient(const double _r, const double _g,
                                   const double _b, const double _a) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetAmbient(const math::Color &_color) {
  throw std::runtime_error("not implemented");
}
math::Color OmniverseMaterial::Diffuse() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetDiffuse(const double _r, const double _g,
                                   const double _b, const double _a) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetDiffuse(const math::Color &_color) {
  throw std::runtime_error("not implemented");
}
math::Color OmniverseMaterial::Specular() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetSpecular(const double _r, const double _g,
                                    const double _b, const double _a) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetSpecular(const math::Color &_color) {
  throw std::runtime_error("not implemented");
}
math::Color OmniverseMaterial::Emissive() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetEmissive(const double _r, const double _g,
                                    const double _b, const double _a) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetEmissive(const math::Color &_color) {
  throw std::runtime_error("not implemented");
}
double OmniverseMaterial::Shininess() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetShininess(const double _shininess) {
  throw std::runtime_error("not implemented");
}
double OmniverseMaterial::Transparency() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetDepthMaterial(const double far, const double near) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetTransparency(const double _transparency) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetAlphaFromTexture(bool _enabled, double _alpha,
                                            bool _twoSided) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::TextureAlphaEnabled() const {
  throw std::runtime_error("not implemented");
}
double OmniverseMaterial::AlphaThreshold() const {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::TwoSidedEnabled() const {
  throw std::runtime_error("not implemented");
}
double OmniverseMaterial::Reflectivity() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetReflectivity(const double _reflectivity) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::CastShadows() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetCastShadows(const bool _castShadows) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::ReceiveShadows() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetReceiveShadows(const bool _receiveShadows) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::ReflectionEnabled() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetReflectionEnabled(const bool _enabled) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasTexture() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::Texture() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetTexture(const std::string &_texture) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearTexture() {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasNormalMap() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::NormalMap() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetNormalMap(const std::string &_normalMap) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearNormalMap() {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasRoughnessMap() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::RoughnessMap() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetRoughnessMap(const std::string &_roughnessMap) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearRoughnessMap() {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasMetalnessMap() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::MetalnessMap() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetMetalnessMap(const std::string &_metalnessMap) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearMetalnessMap() {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasEnvironmentMap() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::EnvironmentMap() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetEnvironmentMap(const std::string &_metalnessMap) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearEnvironmentMap() {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasEmissiveMap() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::EmissiveMap() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetEmissiveMap(const std::string &_emissiveMap) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearEmissiveMap() {
  throw std::runtime_error("not implemented");
}
bool OmniverseMaterial::HasLightMap() const {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::LightMap() const {
  throw std::runtime_error("not implemented");
}
unsigned int OmniverseMaterial::LightMapTexCoordSet() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetLightMap(const std::string &_lightMap,
                                    unsigned int _uvSet) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::ClearLightMap() {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetRenderOrder(const float _renderOrder) {
  throw std::runtime_error("not implemented");
}
float OmniverseMaterial::RenderOrder() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetRoughness(const float _roughness) {
  throw std::runtime_error("not implemented");
}
float OmniverseMaterial::Roughness() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetMetalness(const float _metalness) {
  throw std::runtime_error("not implemented");
}
float OmniverseMaterial::Metalness() const {
  throw std::runtime_error("not implemented");
}
enum MaterialType OmniverseMaterial::Type() const {
  throw std::runtime_error("not implemented");
}
enum ShaderType OmniverseMaterial::ShaderType() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetShaderType(enum ShaderType _type) {
  throw std::runtime_error("not implemented");
}
MaterialPtr OmniverseMaterial::Clone(const std::string &_name) const {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::CopyFrom(ConstMaterialPtr _material) {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::CopyFrom(const common::Material &_material) {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::VertexShader() const {
  throw std::runtime_error("not implemented");
}
ShaderParamsPtr OmniverseMaterial::VertexShaderParams() {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetVertexShader(const std::string &_path) {
  throw std::runtime_error("not implemented");
}
std::string OmniverseMaterial::FragmentShader() const {
  throw std::runtime_error("not implemented");
}
ShaderParamsPtr OmniverseMaterial::FragmentShaderParams() {
  throw std::runtime_error("not implemented");
}
void OmniverseMaterial::SetFragmentShader(const std::string &_path) {
  throw std::runtime_error("not implemented");
}
}  // namespace ignition::rendering::omni
