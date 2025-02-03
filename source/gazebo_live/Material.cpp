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

#include <gz/common/Console.hh>
#include <gz/math/Color.hh>

#include <pxr/usd/usd/tokens.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <OmniClient.h>

namespace gz
{
namespace omniverse
{
namespace
{
/// Return the full path of an URL
/// If the resource is a URI we try to find to file in the filesystem
/// \brief _fullPath URI of the resource
std::string checkURI(const std::string _fullPath)
{
  // TODO (ahcorde): This code is duplicated is the USD converter (sdformat)
  gz::common::URI uri(_fullPath);
  std::string fullPath = _fullPath;
  std::string home;
  if (!gz::common::env("HOME", home, false))
  {
    ignwarn << "The HOME environment variable was not defined, "
            << "so the resource [" << fullPath << "] could not be found\n";
    return "";
  }
  if (uri.Scheme() == "http" || uri.Scheme() == "https")
  {
    auto systemPaths = gz::common::systemPaths();

    std::vector<std::string> tokens = gz::common::split(uri.Path().Str(), "/");
    std::string server = tokens[0];
    std::string versionServer = tokens[1];
    std::string owner = gz::common::lowercase(tokens[2]);
    std::string type = gz::common::lowercase(tokens[3]);
    std::string modelName = gz::common::lowercase(tokens[4]);
    std::string modelVersion = gz::common::lowercase(tokens[5]);

    fullPath = gz::common::joinPaths(
      home, ".gz", "fuel", server, owner, type, modelName, modelVersion);
    systemPaths->AddFilePaths(fullPath);

    for (unsigned int i = 7; i < tokens.size(); i++)
    {
      fullPath = gz::common::joinPaths(
        fullPath, gz::common::lowercase(tokens[i]));
      systemPaths->AddFilePaths(fullPath);
    }
  }
  return fullPath;
}

/// \brief Copy a file in a directory
/// \param[in] _path path where the copy will be located
/// \param[in] _fullPath name of the file to copy
/// \param[in] _stageDirUrl stage directory URL to copy materials if required
bool copyMaterial(
  const std::string &_path,
  const std::string &_fullPath,
  const std::string &_stageDirUrl)
{
  if (!_path.empty() && !_fullPath.empty())
  {
    ///
    auto fileName = gz::common::basename(_path);
    auto filePathIndex = _path.rfind(fileName);
    auto filePath = _path.substr(0, filePathIndex);
    if (!omniClientWaitFor(omniClientCopy(
      _fullPath.c_str(),
      std::string(_stageDirUrl + "/" + _path).c_str(),
      nullptr,
      nullptr), 1000))
    {
      ignerr << "omniClientCopy timeout. Not able to copy file ["
             << _fullPath.c_str() << "]" << "in nucleus ["
             << std::string(_stageDirUrl + "/" + _path) << "]." ;
    }
  }
  return false;
}

/// \brief Create the path to copy the material
/// \param[in] _uri full path of the file to copy
/// \return A relative path to save the material, the path looks like:
/// materials/textures/<filename with extension>
std::string getMaterialCopyPath(const std::string &_uri)
{
return gz::common::joinPaths(
  ".",
  "materials",
  "textures",
  gz::common::basename(_uri));
}

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
}  // namespace

/// \param[in] _stageDirUrl stage directory URL to copy materials if required
bool SetMaterial(const pxr::UsdGeomGprim &_gprim,
                 const gz::msgs::Visual &_visualMsg,
                 const pxr::UsdStageRefPtr &_stage,
                 const std::string &_stageDirUrl)
{
  if (!_visualMsg.has_material())
  {
    return true;
  }

  const std::string mtlPath = "/Looks/Material_" + _visualMsg.name() + "_" +
                              std::to_string(_visualMsg.id());
  pxr::UsdShadeMaterial material =
      pxr::UsdShadeMaterial::Define(_stage, pxr::SdfPath(mtlPath));
  auto usdShader =
      pxr::UsdShadeShader::Define(_stage, pxr::SdfPath(mtlPath + "/Shader"));
  auto shaderPrim = usdShader.GetPrim();

  auto shaderOut =
      pxr::UsdShadeConnectableAPI(shaderPrim)
          .CreateOutput(pxr::TfToken("out"), pxr::SdfValueTypeNames->Token);
  material.CreateSurfaceOutput(pxr::TfToken("mdl")).ConnectToSource(shaderOut);
  material.CreateVolumeOutput(pxr::TfToken("mdl")).ConnectToSource(shaderOut);
  material.CreateDisplacementOutput(pxr::TfToken("mdl"))
      .ConnectToSource(shaderOut);
  usdShader.GetImplementationSourceAttr().Set(pxr::UsdShadeTokens->sourceAsset);
  usdShader.SetSourceAsset(pxr::SdfAssetPath("OmniPBR.mdl"),
                           pxr::TfToken("mdl"));
  usdShader.SetSourceAssetSubIdentifier(pxr::TfToken("OmniPBR"),
                                        pxr::TfToken("mdl"));

  std::map<pxr::TfToken, pxr::VtValue> customDataDiffuse = {
      {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(0.2, 0.2, 0.2))},
      {pxr::TfToken("range:max"),
       pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
      {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}};
  gz::math::Color diffuse(
      _visualMsg.material().diffuse().r(), _visualMsg.material().diffuse().g(),
      _visualMsg.material().diffuse().b(), _visualMsg.material().diffuse().a());
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
  gz::math::Color emissive(_visualMsg.material().emissive().r(),
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

  if (_visualMsg.material().has_pbr())
  {
    auto pbr = _visualMsg.material().pbr();
    std::map<pxr::TfToken, pxr::VtValue> customDataMetallicConstant =
    {
      {pxr::TfToken("default"), pxr::VtValue(0.5)},
      {pxr::TfToken("range:max"), pxr::VtValue(1)},
      {pxr::TfToken("range:min"), pxr::VtValue(0)}
    };
    CreateMaterialInput<float>(
      shaderPrim,
      "metallic_constant",
      pxr::SdfValueTypeNames->Float,
      pbr.metalness(),
      customDataMetallicConstant,
      pxr::TfToken("Metallic Amount"),
      pxr::TfToken("Reflectivity"),
      "Metallic Material");
    std::map<pxr::TfToken, pxr::VtValue> customDataRoughnessConstant =
    {
      {pxr::TfToken("default"), pxr::VtValue(0.5)},
      {pxr::TfToken("range:max"), pxr::VtValue(1)},
      {pxr::TfToken("range:min"), pxr::VtValue(0)}
    };
    CreateMaterialInput<float>(
      shaderPrim,
      "reflection_roughness_constant",
      pxr::SdfValueTypeNames->Float,
      pbr.roughness(),
      customDataRoughnessConstant,
      pxr::TfToken("Roughness Amount"),
      pxr::TfToken("Reflectivity"),
      "Higher roughness values lead to more blurry reflections");
    if (!pbr.albedo_map().empty())
    {
      std::map<pxr::TfToken, pxr::VtValue> customDataDiffuseTexture =
      {
        {pxr::TfToken("default"), pxr::VtValue(pxr::SdfAssetPath())},
      };

      std::string copyPath = getMaterialCopyPath(pbr.albedo_map());

      std::string albedoMapURI = checkURI(pbr.albedo_map());

      std::string fullnameAlbedoMap =
        gz::common::findFile(
          gz::common::basename(albedoMapURI));

      if (fullnameAlbedoMap.empty())
      {
        fullnameAlbedoMap = pbr.albedo_map();
      }

      copyMaterial(copyPath, fullnameAlbedoMap, _stageDirUrl);

      CreateMaterialInput<pxr::SdfAssetPath>(
        shaderPrim,
        "diffuse_texture",
        pxr::SdfValueTypeNames->Asset,
        pxr::SdfAssetPath(copyPath),
        customDataDiffuseTexture,
        pxr::TfToken("Base Map"),
        pxr::TfToken("Albedo"),
        "",
        pxr::TfToken("auto"));
    }
    if (!pbr.metalness_map().empty())
    {
      std::map<pxr::TfToken, pxr::VtValue> customDataMetallnessTexture =
      {
        {pxr::TfToken("default"), pxr::VtValue(pxr::SdfAssetPath())},
      };

      std::string copyPath = getMaterialCopyPath(pbr.metalness_map());

      std::string fullnameMetallnessMap =
        gz::common::findFile(
          gz::common::basename(pbr.metalness_map()));

      if (fullnameMetallnessMap.empty())
      {
        fullnameMetallnessMap = pbr.metalness_map();
      }

      copyMaterial(copyPath, fullnameMetallnessMap, _stageDirUrl);

      CreateMaterialInput<pxr::SdfAssetPath>(
        shaderPrim,
        "metallic_texture",
        pxr::SdfValueTypeNames->Asset,
        pxr::SdfAssetPath(copyPath),
        customDataMetallnessTexture,
        pxr::TfToken("Metallic Map"),
        pxr::TfToken("Reflectivity"),
        "",
        pxr::TfToken("raw"));
    }
    if (!pbr.normal_map().empty())
    {
      std::map<pxr::TfToken, pxr::VtValue> customDataNormalTexture =
      {
        {pxr::TfToken("default"), pxr::VtValue(pxr::SdfAssetPath())},
      };

      std::string copyPath = getMaterialCopyPath(pbr.normal_map());

      std::string fullnameNormalMap =
        gz::common::findFile(
          gz::common::basename(pbr.normal_map()));

      if (fullnameNormalMap.empty())
      {
        fullnameNormalMap = pbr.normal_map();
      }

      copyMaterial(copyPath, fullnameNormalMap, _stageDirUrl);

      CreateMaterialInput<pxr::SdfAssetPath>(
        shaderPrim,
        "normalmap_texture",
        pxr::SdfValueTypeNames->Asset,
        pxr::SdfAssetPath(copyPath),
        customDataNormalTexture,
        pxr::TfToken("Normal Map"),
        pxr::TfToken("Normal"),
        "",
        pxr::TfToken("raw"));
    }
    if (!pbr.roughness_map().empty())
    {
      std::map<pxr::TfToken, pxr::VtValue> customDataRoughnessTexture =
      {
        {pxr::TfToken("default"), pxr::VtValue(pxr::SdfAssetPath())},
      };

      std::string copyPath = getMaterialCopyPath(pbr.roughness_map());

      std::string fullnameRoughnessMap =
        gz::common::findFile(
          gz::common::basename(pbr.roughness_map()));

      if (fullnameRoughnessMap.empty())
      {
        fullnameRoughnessMap = pbr.roughness_map();
      }

      copyMaterial(copyPath, fullnameRoughnessMap, _stageDirUrl);

      CreateMaterialInput<pxr::SdfAssetPath>(
        shaderPrim,
        "reflectionroughness_texture",
        pxr::SdfValueTypeNames->Asset,
        pxr::SdfAssetPath(copyPath),
        customDataRoughnessTexture,
        pxr::TfToken("RoughnessMap Map"),
        pxr::TfToken("RoughnessMap"),
        "",
        pxr::TfToken("raw"));

      std::map<pxr::TfToken, pxr::VtValue>
        customDataRoughnessTextureInfluence =
      {
        {pxr::TfToken("default"), pxr::VtValue(0)},
        {pxr::TfToken("range:max"), pxr::VtValue(1)},
        {pxr::TfToken("range:min"), pxr::VtValue(0)}
      };

      CreateMaterialInput<bool>(
        shaderPrim,
        "reflection_roughness_texture_influence",
        pxr::SdfValueTypeNames->Bool,
        true,
        customDataRoughnessTextureInfluence,
        pxr::TfToken("Roughness Map Influence"),
        pxr::TfToken("Reflectivity"),
        "",
        pxr::TfToken("raw"));
    }
  }
  pxr::UsdShadeMaterialBindingAPI(_gprim).Bind(material);
  return true;
}
}  // namespace omniverse
}  // namespace gz
