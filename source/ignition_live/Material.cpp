/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include "Material.hpp"

#include <ignition/common/Console.hh>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <pxr/usd/usd/tokens.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdShade/material.h>

namespace ignition
{
namespace omniverse
{
/// \brief Fill Material shader attributes and properties
/// \param[in] _prim USD primitive
/// \param[in] _name Name of the field attribute or property
/// \param[in] _vType Type of the field
/// \param[in] _value Value of the field
/// \param[in] _customData Custom data to set the field
/// \param[in] _displayName Display name
/// \param[in] _displayGroup Display group
/// \param[in] _doc Documentation of the field
/// \param[in] _colorSpace if the material is a texture, we can specify the
/// colorSpace of the image
template <typename T>
void CreateMaterialInput(
    const pxr::UsdPrim &_prim, const std::string &_name,
    const pxr::SdfValueTypeName &_vType, T _value,
    const std::map<pxr::TfToken, pxr::VtValue> &_customData,
    const pxr::TfToken &_displayName = pxr::TfToken(""),
    const pxr::TfToken &_displayGroup = pxr::TfToken(""),
    const std::string &_doc = "",
    const pxr::TfToken &_colorSpace = pxr::TfToken(""))
{
  auto shader = pxr::UsdShadeShader(_prim);
  if (shader)
  {
    auto existingInput = shader.GetInput(pxr::TfToken(_name));
    pxr::SdfValueTypeName vTypeName;
    if (_vType.IsScalar())
    {
      vTypeName = _vType.GetScalarType();
    }
    else if (_vType.IsArray())
    {
      vTypeName = _vType.GetArrayType();
    }
    auto surfaceInput = shader.CreateInput(pxr::TfToken(_name), vTypeName);
    surfaceInput.Set(_value);
    auto attr = surfaceInput.GetAttr();

    for (const auto &[key, customValue] : _customData)
    {
      attr.SetCustomDataByKey(key, customValue);
    }
    if (!_displayName.GetString().empty())
    {
      attr.SetDisplayName(_displayName);
    }
    if (!_displayGroup.GetString().empty())
    {
      attr.SetDisplayGroup(_displayGroup);
    }
    if (!_doc.empty())
    {
      attr.SetDocumentation(_doc);
    }
    if (!_colorSpace.GetString().empty())
    {
      attr.SetColorSpace(_colorSpace);
    }
  }
  else
  {
    ignerr << "Not able to convert the prim to a UsdShadeShader" << std::endl;
  }
}

pxr::UsdShadeMaterial ParseMaterial(const ignition::msgs::Visual &_visualMsg,
                                    Scene::SharedPtr &_scene)
{
  const std::string mtl_path = "/Looks/Material_" + _visualMsg.name();
  pxr::UsdShadeMaterial material;
  material = _scene->CreateMaterial(mtl_path);
  auto usdShader = _scene->CreateShader(mtl_path + "/Shader");

  auto shaderPrim = _scene->GetPrimAtPath(mtl_path + "/Shader");
  if (!shaderPrim)
  {
    ignerr << "Not able to cast the UsdShadeShader to a Prim" << std::endl;
  }
  else
  {
    auto shader_out =
        pxr::UsdShadeConnectableAPI(shaderPrim)
            .CreateOutput(pxr::TfToken("out"), pxr::SdfValueTypeNames->Token);
    material.CreateSurfaceOutput(pxr::TfToken("mdl"))
        .ConnectToSource(shader_out);
    material.CreateVolumeOutput(pxr::TfToken("mdl"))
        .ConnectToSource(shader_out);
    material.CreateDisplacementOutput(pxr::TfToken("mdl"))
        .ConnectToSource(shader_out);
    usdShader.GetImplementationSourceAttr().Set(
        pxr::UsdShadeTokens->sourceAsset);
    usdShader.SetSourceAsset(pxr::SdfAssetPath("OmniPBR.mdl"),
                             pxr::TfToken("mdl"));
    usdShader.SetSourceAssetSubIdentifier(pxr::TfToken("OmniPBR"),
                                          pxr::TfToken("mdl"));

    std::map<pxr::TfToken, pxr::VtValue> customDataDiffuse = {
        {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(0.2, 0.2, 0.2))},
        {pxr::TfToken("range:max"),
         pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
        {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}};
    ignition::math::Color diffuse(_visualMsg.material().diffuse().r(),
                                  _visualMsg.material().diffuse().g(),
                                  _visualMsg.material().diffuse().b(),
                                  _visualMsg.material().diffuse().a());
    CreateMaterialInput<pxr::GfVec3f>(
        shaderPrim, "diffuse_color_constant", pxr::SdfValueTypeNames->Color3f,
        pxr::GfVec3f(diffuse.R(), diffuse.G(), diffuse.B()), customDataDiffuse,
        pxr::TfToken("Base Color"), pxr::TfToken("Albedo"),
        "This is the base color");

    std::map<pxr::TfToken, pxr::VtValue> customDataEmissive = {
        {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(1, 0.1, 0.1))},
        {pxr::TfToken("range:max"),
         pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
        {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}};
    ignition::math::Color emissive(_visualMsg.material().emissive().r(),
                                   _visualMsg.material().emissive().g(),
                                   _visualMsg.material().emissive().b(),
                                   _visualMsg.material().emissive().a());
    CreateMaterialInput<pxr::GfVec3f>(
        shaderPrim, "emissive_color", pxr::SdfValueTypeNames->Color3f,
        pxr::GfVec3f(emissive.R(), emissive.G(), emissive.B()),
        customDataEmissive, pxr::TfToken("Emissive Color"),
        pxr::TfToken("Emissive"), "The emission color");

    std::map<pxr::TfToken, pxr::VtValue> customDataEnableEmission = {
        {pxr::TfToken("default"), pxr::VtValue(0)}};

    CreateMaterialInput<bool>(
        shaderPrim, "enable_emission", pxr::SdfValueTypeNames->Bool,
        emissive.A() > 0, customDataEnableEmission,
        pxr::TfToken("Enable Emissive"), pxr::TfToken("Emissive"),
        "Enables the emission of light from the material");

    std::map<pxr::TfToken, pxr::VtValue> customDataIntensity = {
        {pxr::TfToken("default"), pxr::VtValue(40)},
        {pxr::TfToken("range:max"), pxr::VtValue(100000)},
        {pxr::TfToken("range:min"), pxr::VtValue(0)}};
    CreateMaterialInput<float>(
        shaderPrim, "emissive_intensity", pxr::SdfValueTypeNames->Float,
        emissive.A(), customDataIntensity, pxr::TfToken("Emissive Intensity"),
        pxr::TfToken("Emissive"), "Intensity of the emission");
  }
  return material;
}
}  // namespace omniverse
}  // namespace ignition
