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

#include "Mesh.hpp"
#include "Util.hpp"

#include <gz/common/Console.hh>
#include <gz/common/Material.hh>
#include <gz/common/Mesh.hh>
#include <gz/common/MeshManager.hh>
#include <gz/common/Filesystem.hh>
#include <gz/common/SubMesh.hh>
#include <gz/common/URI.hh>
#include <gz/common/Util.hh>

#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "sdf/Material.hh"
#include "sdf/Pbr.hh"

namespace gz::omniverse
{
namespace {
bool endsWith(const std::string_view &str, const std::string_view &suffix)
{
  return str.size() >= suffix.size() &&
         0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

/// \brief Copy a file in a directory
/// \param[in] _path path where the copy will be located
/// \param[in] _fullPath name of the file to copy
/// \return True if the file at _fullPath was copied to _path. False otherwise
bool copyMaterial(const std::string &_path, const std::string &_fullPath)
{
  if (!_path.empty() && !_fullPath.empty())
  {
    auto fileName = gz::common::basename(_path);
    auto filePathIndex = _path.rfind(fileName);
    auto filePath = _path.substr(0, filePathIndex);
    if (!filePath.empty())
    {
      gz::common::createDirectories(filePath);
    }
    return gz::common::copyFile(_fullPath, _path);
  }
  return false;
}

/// \brief Get the path to copy the material to
/// \param[in] _uri full path of the file to copy
/// \return A relative path to save the material. The path looks like:
/// materials/textures/<filename with extension>
std::string getMaterialCopyPath(const std::string &_uri)
{
  return gz::common::joinPaths(
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
/// color space of the image
/// \return gz::usd::UsdErrors, which is a list of UsdError objects.
/// This list is empty if no errors occurred when creating the material input.
template<typename T>
bool CreateMaterialInput(
  const pxr::UsdPrim &_prim,
  const std::string &_name,
  const pxr::SdfValueTypeName &_vType,
  const T &_value,
  const std::map<pxr::TfToken, pxr::VtValue> &_customData,
  const pxr::TfToken &_displayName,
  const pxr::TfToken &_displayGroup,
  const std::string &_doc,
  const pxr::TfToken &_colorSpace = pxr::TfToken(""))
{
  auto shader = pxr::UsdShadeShader(_prim);
  if (!shader)
  {
    gzerr << "Unable to convert the prim to a UsdShadeShader" << std::endl;
    return false;
  }

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
  auto surfaceInput = shader.CreateInput(
    pxr::TfToken(_name), vTypeName);
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
  return true;
}

bool ParseSdfMaterial(const sdf::Material *_materialSdf,
    const pxr::UsdStageRefPtr &_stage, pxr::SdfPath &_materialPath)
{
  const auto looksPath = pxr::SdfPath("/Looks");
  auto looksPrim = _stage->GetPrimAtPath(looksPath);
  if (!looksPrim)
  {
    looksPrim = _stage->DefinePrim(looksPath, pxr::TfToken("Scope"));
  }

  // This variable will increase with every new material to avoid collision
  // with the names of the materials
  static int i = 0;

  _materialPath = pxr::SdfPath("/Looks/Material_" + std::to_string(i));
  i++;

  pxr::UsdShadeMaterial materialUsd;
  auto usdMaterialPrim = _stage->GetPrimAtPath(_materialPath);
  if (!usdMaterialPrim)
  {
    materialUsd = pxr::UsdShadeMaterial::Define(_stage, _materialPath);
  }
  else
  {
    materialUsd = pxr::UsdShadeMaterial(usdMaterialPrim);
  }

  const auto shaderPath = pxr::SdfPath(_materialPath.GetString() + "/Shader");
  auto usdShader = pxr::UsdShadeShader::Define(_stage, shaderPath);
  auto shaderPrim = _stage->GetPrimAtPath(shaderPath);
  if (!shaderPrim)
  {
    gzerr << "Not able to cast the UsdShadeShader at path [" + shaderPath.GetString()
      + "] to a Prim" << std::endl;
    return false;
  }

  auto shaderOut = pxr::UsdShadeConnectableAPI(shaderPrim).CreateOutput(
    pxr::TfToken("out"), pxr::SdfValueTypeNames->Token);
  const auto mdlToken = pxr::TfToken("mdl");
  materialUsd.CreateSurfaceOutput(mdlToken).ConnectToSource(shaderOut);
  materialUsd.CreateVolumeOutput(mdlToken).ConnectToSource(shaderOut);
  materialUsd.CreateDisplacementOutput(mdlToken).ConnectToSource(shaderOut);
  usdShader.GetImplementationSourceAttr().Set(
    pxr::UsdShadeTokens->sourceAsset);
  usdShader.SetSourceAsset(pxr::SdfAssetPath("OmniPBR.mdl"), mdlToken);
  usdShader.SetSourceAssetSubIdentifier(pxr::TfToken("OmniPBR"), mdlToken);

  const std::map<pxr::TfToken, pxr::VtValue> customDataDiffuse =
  {
    {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(0.2, 0.2, 0.2))},
    {pxr::TfToken("range:max"),
      pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
    {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}
  };
  const math::Color diffuse = _materialSdf->Diffuse();

  bool success = CreateMaterialInput<pxr::GfVec3f>(
    shaderPrim,
    "diffuse_color_constant",
    pxr::SdfValueTypeNames->Color3f,
    pxr::GfVec3f(diffuse.R(), diffuse.G(), diffuse.B()),
    customDataDiffuse,
    pxr::TfToken("Base Color"),
    pxr::TfToken("Albedo"),
    "This is the base color");

  if (!success)
  {
    gzerr << "Unable to set the base color of the material at path ["
          + _materialPath.GetString() + "]" << std::endl;
    return false;
  }

  const std::map<pxr::TfToken, pxr::VtValue> customDataEmissive =
  {
    {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(1, 0.1, 0.1))},
    {pxr::TfToken("range:max"),
      pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
    {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}
  };
  math::Color emissive = _materialSdf->Emissive();
  success = CreateMaterialInput<pxr::GfVec3f>(
    shaderPrim,
    "emissive_color",
    pxr::SdfValueTypeNames->Color3f,
    pxr::GfVec3f(emissive.R(), emissive.G(), emissive.B()),
    customDataEmissive,
    pxr::TfToken("Emissive Color"),
    pxr::TfToken("Emissive"),
    "The emission color");

  if (!success)
  {
    gzerr << "Unable to set the emission color of the material at path ["
          + _materialPath.GetString() + "]" << std::endl;
    return false;
  }

  const std::map<pxr::TfToken, pxr::VtValue> customDataEnableEmission =
  {
    {pxr::TfToken("default"), pxr::VtValue(0)}
  };

  success = CreateMaterialInput<bool>(
    shaderPrim,
    "enable_emission",
    pxr::SdfValueTypeNames->Bool,
    emissive.A() > 0,
    customDataEnableEmission,
    pxr::TfToken("Enable Emissive"),
    pxr::TfToken("Emissive"),
    "Enables the emission of light from the material");

  if (!success)
  {
    gzerr << "Unable to set the emissive enabled propery of the material at path"
          " [" + _materialPath.GetString() + "]" << std::endl;
    return false;
  }

  const std::map<pxr::TfToken, pxr::VtValue> customDataIntensity =
  {
    {pxr::TfToken("default"), pxr::VtValue(40)},
    {pxr::TfToken("range:max"), pxr::VtValue(100000)},
    {pxr::TfToken("range:min"), pxr::VtValue(0)}
  };
  success = CreateMaterialInput<float>(
    shaderPrim,
    "emissive_intensity",
    pxr::SdfValueTypeNames->Float,
    emissive.A(),
    customDataIntensity,
    pxr::TfToken("Emissive Intensity"),
    pxr::TfToken("Emissive"),
    "Intensity of the emission");

  if (!success)
  {
    gzerr << "Unable to set the emissive intensity of the material at path ["
          + _materialPath.GetString() + "]" << std::endl;
    return false;
  }

  const sdf::Pbr * pbr = _materialSdf->PbrMaterial();
  if (pbr)
  {
    const sdf::PbrWorkflow * pbrWorkflow =
      pbr->Workflow(sdf::PbrWorkflowType::METAL);
    if (!pbrWorkflow)
    {
      pbrWorkflow = pbr->Workflow(sdf::PbrWorkflowType::SPECULAR);
    }

    if (pbrWorkflow)
    {
      const std::map<pxr::TfToken, pxr::VtValue> customDataMetallicConstant =
      {
        {pxr::TfToken("default"), pxr::VtValue(0.5)},
        {pxr::TfToken("range:max"), pxr::VtValue(1)},
        {pxr::TfToken("range:min"), pxr::VtValue(0)}
      };
      success = CreateMaterialInput<float>(
        shaderPrim,
        "metallic_constant",
        pxr::SdfValueTypeNames->Float,
        pbrWorkflow->Metalness(),
        customDataMetallicConstant,
        pxr::TfToken("Metallic Amount"),
        pxr::TfToken("Reflectivity"),
        "Metallic Material");
      if (!success)
      {
        gzerr << "Unable to set the metallic constant of the material at path ["
          + _materialPath.GetString() + "]" << std::endl;
        return false;
      }
      const std::map<pxr::TfToken, pxr::VtValue> customDataRoughnessConstant =
      {
        {pxr::TfToken("default"), pxr::VtValue(0.5)},
        {pxr::TfToken("range:max"), pxr::VtValue(1)},
        {pxr::TfToken("range:min"), pxr::VtValue(0)}
      };
      success =
        CreateMaterialInput<float>(
          shaderPrim,
          "reflection_roughness_constant",
          pxr::SdfValueTypeNames->Float,
          pbrWorkflow->Roughness(),
          customDataRoughnessConstant,
          pxr::TfToken("Roughness Amount"),
          pxr::TfToken("Reflectivity"),
          "Higher roughness values lead to more blurry reflections");
      if (!success)
      {
        gzerr << "Unable to set the roughness constant of the material at path ["
          + _materialPath.GetString() + "]" << std::endl;
        return false;
      }

      const std::map<pxr::TfToken, pxr::VtValue> customDefaultSdfAssetPath =
      {
        {pxr::TfToken("default"), pxr::VtValue(pxr::SdfAssetPath())},
      };

      if (!pbrWorkflow->AlbedoMap().empty())
      {
        std::string copyPath = getMaterialCopyPath(pbrWorkflow->AlbedoMap());

        std::string fullnameAlbedoMap =
          gz::common::findFile(
            gz::common::basename(pbrWorkflow->AlbedoMap()));

        if (fullnameAlbedoMap.empty())
        {
          fullnameAlbedoMap = pbrWorkflow->AlbedoMap();
        }

        copyMaterial(copyPath, fullnameAlbedoMap);

        success =
          CreateMaterialInput<pxr::SdfAssetPath>(
            shaderPrim,
            "diffuse_texture",
            pxr::SdfValueTypeNames->Asset,
            pxr::SdfAssetPath(copyPath),
            customDefaultSdfAssetPath,
            pxr::TfToken("Base Map"),
            pxr::TfToken("Albedo"),
            "",
            pxr::TfToken("auto"));
        if (!success)
        {
          gzerr << "Unable to set the albedo of the material at path ["
            + _materialPath.GetString() + "]" << std::endl;
          return false;
        }
      }
      if (!pbrWorkflow->MetalnessMap().empty())
      {
        std::string copyPath =
          getMaterialCopyPath(pbrWorkflow->MetalnessMap());

        std::string fullnameMetallnessMap =
          gz::common::findFile(
            gz::common::basename(pbrWorkflow->MetalnessMap()));

        if (fullnameMetallnessMap.empty())
        {
          fullnameMetallnessMap = pbrWorkflow->MetalnessMap();
        }

        copyMaterial(copyPath, fullnameMetallnessMap);

        success =
          CreateMaterialInput<pxr::SdfAssetPath>(
            shaderPrim,
            "metallic_texture",
            pxr::SdfValueTypeNames->Asset,
            pxr::SdfAssetPath(copyPath),
            customDefaultSdfAssetPath,
            pxr::TfToken("Metallic Map"),
            pxr::TfToken("Reflectivity"),
            "",
            pxr::TfToken("raw"));
        if (!success)
        {
          gzerr << "Unable to set the reflectivity of the material at path ["
            + _materialPath.GetString() + "]" << std::endl;
          return false;
        }
      }
      if (!pbrWorkflow->NormalMap().empty())
      {
        std::string copyPath = getMaterialCopyPath(pbrWorkflow->NormalMap());

        std::string fullnameNormalMap =
          gz::common::findFile(
            gz::common::basename(pbrWorkflow->NormalMap()));

        if (fullnameNormalMap.empty())
        {
          fullnameNormalMap = pbrWorkflow->NormalMap();
        }

        copyMaterial(copyPath, fullnameNormalMap);

        success =
          CreateMaterialInput<pxr::SdfAssetPath>(
            shaderPrim,
            "normalmap_texture",
            pxr::SdfValueTypeNames->Asset,
            pxr::SdfAssetPath(copyPath),
            customDefaultSdfAssetPath,
            pxr::TfToken("Normal Map"),
            pxr::TfToken("Normal"),
            "",
            pxr::TfToken("raw"));
        if (!success)
        {
          gzerr << "Unable to set the normal map of the material at path ["
            + _materialPath.GetString() + "]" << std::endl;
          return false;
        }
      }
      if (!pbrWorkflow->RoughnessMap().empty())
      {
        std::string copyPath =
          getMaterialCopyPath(pbrWorkflow->RoughnessMap());

        std::string fullnameRoughnessMap =
          gz::common::findFile(
            gz::common::basename(pbrWorkflow->RoughnessMap()));

        if (fullnameRoughnessMap.empty())
        {
          fullnameRoughnessMap = pbrWorkflow->RoughnessMap();
        }

        copyMaterial(copyPath, fullnameRoughnessMap);

        success =
          CreateMaterialInput<pxr::SdfAssetPath>(
            shaderPrim,
            "reflectionroughness_texture",
            pxr::SdfValueTypeNames->Asset,
            pxr::SdfAssetPath(copyPath),
            customDefaultSdfAssetPath,
            pxr::TfToken("RoughnessMap Map"),
            pxr::TfToken("RoughnessMap"),
            "",
            pxr::TfToken("raw"));
        if (!success)
        {
          gzerr << "Unable to set the roughness map of the material at path ["
            + _materialPath.GetString() + "]" << std::endl;
          return false;
        }

        const std::map<pxr::TfToken, pxr::VtValue>
          customDataRoughnessTextureInfluence =
        {
          {pxr::TfToken("default"), pxr::VtValue(0)},
          {pxr::TfToken("range:max"), pxr::VtValue(1)},
          {pxr::TfToken("range:min"), pxr::VtValue(0)}
        };

        success =
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
        if (!success)
        {
          gzerr << "Unable to set the reflectivity of the material at path ["
            + _materialPath.GetString() + "]" << std::endl;
          return false;
        }
      }
    }
  }

  return true;
}

sdf::Material convert(const gz::common::Material *_in)
{
  sdf::Material out;
  out.SetEmissive(_in->Emissive());
  out.SetDiffuse(_in->Diffuse());
  out.SetSpecular(_in->Specular());
  out.SetAmbient(_in->Ambient());
  out.SetRenderOrder(_in->RenderOrder());
  out.SetLighting(_in->Lighting());
  out.SetDoubleSided(_in->TwoSidedEnabled());
  const gz::common::Pbr * pbr = _in->PbrMaterial();
  if (pbr != nullptr)
  {
    out.SetNormalMap(pbr->NormalMap());
    sdf::Pbr pbrOut;
    sdf::PbrWorkflow pbrWorkflow;
    if (!pbr->AlbedoMap().empty())
    {
      pbrWorkflow.SetAlbedoMap(pbr->AlbedoMap());
    }
    else
    {
      pbrWorkflow.SetAlbedoMap(_in->TextureImage());
    }
    pbrWorkflow.SetMetalnessMap(pbr->MetalnessMap());
    pbrWorkflow.SetEmissiveMap(pbr->EmissiveMap());
    pbrWorkflow.SetRoughnessMap(pbr->RoughnessMap());
    pbrWorkflow.SetSpecularMap(pbr->SpecularMap());
    pbrWorkflow.SetEnvironmentMap(pbr->EnvironmentMap());
    pbrWorkflow.SetAmbientOcclusionMap(pbr->AmbientOcclusionMap());
    pbrWorkflow.SetLightMap(pbr->LightMap());
    pbrWorkflow.SetRoughness(pbr->Roughness());
    pbrWorkflow.SetGlossiness(pbr->Glossiness());
    pbrWorkflow.SetMetalness(pbr->Metalness());

    if (pbr->NormalMapType() == gz::common::NormalMapSpace::TANGENT)
    {
      pbrWorkflow.SetNormalMap(
        pbr->NormalMap(), sdf::NormalMapSpace::TANGENT);
    }
    else
    {
      pbrWorkflow.SetNormalMap(
        pbr->NormalMap(), sdf::NormalMapSpace::OBJECT);
    }

    if (pbr->Type() == gz::common::PbrType::SPECULAR)
    {
      pbrWorkflow.SetType(sdf::PbrWorkflowType::SPECULAR);
      pbrOut.SetWorkflow(sdf::PbrWorkflowType::SPECULAR, pbrWorkflow);
    }
    else
    {
      pbrWorkflow.SetType(sdf::PbrWorkflowType::METAL);
      pbrOut.SetWorkflow(sdf::PbrWorkflowType::METAL, pbrWorkflow);
    }
    out.SetPbrMaterial(pbrOut);
  }
  else if (!_in->TextureImage().empty())
  {
    sdf::Pbr pbrOut;
    sdf::PbrWorkflow pbrWorkflow;
    pbrWorkflow.SetAlbedoMap(_in->TextureImage());
    pbrOut.SetWorkflow(sdf::PbrWorkflowType::SPECULAR, pbrWorkflow);
    out.SetPbrMaterial(pbrOut);
  }

  return out;
}

}  // namespace

bool UpdateMesh(const gz::msgs::MeshGeom &_meshMsg,
                            const std::string &_path,
                            const pxr::UsdStageRefPtr &_stage)
{
  gz::common::URI uri(_meshMsg.filename());
  std::string fullname;

  std::string home;
  if (!gz::common::env("HOME", home, false))
  {
    ignerr << "The HOME environment variable was not defined, "
           << "so the resource [" << fullname << "] could not be found\n";
    return false;
  }
  if (uri.Scheme() == "https" || uri.Scheme() == "http")
  {
    auto systemPaths = gz::common::systemPaths();

    std::vector<std::string> tokens = gz::common::split(uri.Path().Str(), "/");
    std::string server = tokens[0];
    std::string versionServer = tokens[1];
    std::string owner = gz::common::lowercase(tokens[2]);
    std::string type = gz::common::lowercase(tokens[3]);
    std::string modelName = gz::common::lowercase(tokens[4]);
    std::string modelVersion = gz::common::lowercase(tokens[5]);

    fullname = gz::common::joinPaths(
      home, ".gz", "fuel", server, owner, type, modelName, modelVersion);
    systemPaths->AddFilePaths(fullname);

    for (unsigned int i = 7; i < tokens.size(); i++)
    {
      fullname = gz::common::joinPaths(
        fullname, gz::common::lowercase(tokens[i]));
      systemPaths->AddFilePaths(fullname);
    }
  }
  else
  {
    fullname = gz::common::findFile(_meshMsg.filename());
    if (fullname.empty()) {
      ignmsg << "Couldn't find file at " << _meshMsg.filename()
        << ". Trying again with just the basename." << std::endl;
      fullname = gz::common::findFile(gz::common::basename(_meshMsg.filename()));
      if (fullname.empty()) {
        ignerr << "Couldn't locate file." << std::endl;
        return false;
      }
    }
    ignmsg << "Found mesh file " << _meshMsg.filename() << " at " << fullname << std::endl;
  }

  const gz::common::Mesh* ignMesh = gz::common::MeshManager::Instance()->Load(fullname);

  // Some Meshes are splited in some submeshes, this loop check if the name
  // of the path is the same as the name of the submesh. In this case
  // we create a USD mesh per submesh.
  bool isUSDPathInSubMeshName = false;
  for (unsigned int i = 0; i < ignMesh->SubMeshCount(); ++i)
  {
    auto subMesh = ignMesh->SubMeshByIndex(i).lock();

    if (ignMesh->SubMeshCount() != 1)
    {
      std::string pathLowerCase = gz::common::lowercase(_path);
      std::string subMeshLowerCase =
          gz::common::lowercase(subMesh->Name());

      if (pathLowerCase.find(subMeshLowerCase) != std::string::npos)
      {
        isUSDPathInSubMeshName = true;
        break;
      }
    }
  }

  for (unsigned int i = 0; i < ignMesh->SubMeshCount(); ++i)
  {
    pxr::VtArray<pxr::GfVec3f> meshPoints;
    pxr::VtArray<pxr::GfVec2f> uvs;
    pxr::VtArray<pxr::GfVec3f> normals;
    pxr::VtArray<int> faceVertexIndices;
    pxr::VtArray<int> faceVertexCounts;

    auto subMesh = ignMesh->SubMeshByIndex(i).lock();
    if (!subMesh)
    {
      ignerr << "Unable to get a shared pointer to submesh at index [" << i
             << "] of parent mesh [" << ignMesh->Name() << "]" << std::endl;
      return false;
    }
    if (isUSDPathInSubMeshName)
    {
      if (ignMesh->SubMeshCount() != 1)
      {
        std::string pathLowerCase = gz::common::lowercase(_path);
        std::string subMeshLowerCase =
            gz::common::lowercase(subMesh->Name());

        if (pathLowerCase.find(subMeshLowerCase) == std::string::npos)
        {
          continue;
        }
      }
    }
    // copy the submesh's vertices to the usd mesh's "points" array
    for (unsigned int v = 0; v < subMesh->VertexCount(); ++v)
    {
      const auto &vertex = subMesh->Vertex(v);
      meshPoints.push_back(pxr::GfVec3f(vertex.X(), vertex.Y(), vertex.Z()));
    }

    // copy the submesh's indices to the usd mesh's "faceVertexIndices" array
    for (unsigned int j = 0; j < subMesh->IndexCount(); ++j)
      faceVertexIndices.push_back(subMesh->Index(j));

    // copy the submesh's texture coordinates
    for (unsigned int j = 0; j < subMesh->TexCoordCount(); ++j)
    {
      const auto &uv = subMesh->TexCoord(j);
      uvs.push_back(pxr::GfVec2f(uv[0], 1 - uv[1]));
    }

    // copy the submesh's normals
    for (unsigned int j = 0; j < subMesh->NormalCount(); ++j)
    {
      const auto &normal = subMesh->Normal(j);
      normals.push_back(pxr::GfVec3f(normal[0], normal[1], normal[2]));
    }

    // set the usd mesh's "faceVertexCounts" array according to
    // the submesh primitive type
    // TODO(adlarkin) support all primitive types. The computations are more
    // involved for LINESTRIPS, TRIFANS, and TRISTRIPS. I will need to spend
    // some time deriving what the number of faces for these primitive types
    // are, given the number of indices. The "faceVertexCounts" array will
    // also not have the same value for every element in the array for these
    // more complex primitive types (see the TODO note in the for loop below)
    unsigned int verticesPerFace = 0;
    unsigned int numFaces = 0;
    switch (subMesh->SubMeshPrimitiveType())
    {
      case gz::common::SubMesh::PrimitiveType::POINTS:
        verticesPerFace = 1;
        numFaces = subMesh->IndexCount();
        break;
      case gz::common::SubMesh::PrimitiveType::LINES:
        verticesPerFace = 2;
        numFaces = subMesh->IndexCount() / 2;
        break;
      case gz::common::SubMesh::PrimitiveType::TRIANGLES:
        verticesPerFace = 3;
        numFaces = subMesh->IndexCount() / 3;
        break;
      case gz::common::SubMesh::PrimitiveType::LINESTRIPS:
      case gz::common::SubMesh::PrimitiveType::TRIFANS:
      case gz::common::SubMesh::PrimitiveType::TRISTRIPS:
      default:
        ignerr << "Submesh " << subMesh->Name()
               << " has a primitive type that is not supported." << std::endl;
        return false;
    }
    // TODO(adlarkin) update this loop to allow for varying element
    // values in the array (see TODO note above). Right now, the
    // array only allows for all elements to have one value, which in
    // this case is "verticesPerFace"
    for (unsigned int n = 0; n < numFaces; ++n)
      faceVertexCounts.push_back(verticesPerFace);

    std::string primName = _path + "/" + subMesh->Name();
    primName = validPath(primName);

    if (endsWith(primName, "/"))
    {
      primName.erase(primName.size() - 1);
    }

    auto usdMesh = pxr::UsdGeomMesh::Define(_stage, pxr::SdfPath(primName));
    usdMesh.CreatePointsAttr().Set(meshPoints);
    usdMesh.CreateFaceVertexIndicesAttr().Set(faceVertexIndices);
    usdMesh.CreateFaceVertexCountsAttr().Set(faceVertexCounts);

    auto coordinates = pxr::UsdGeomPrimvarsAPI(usdMesh).CreatePrimvar(
        pxr::TfToken("st"), pxr::SdfValueTypeNames->Float2Array,
        pxr::UsdGeomTokens->vertex);
    coordinates.Set(uvs);

    usdMesh.CreateNormalsAttr().Set(normals);
    usdMesh.SetNormalsInterpolation(pxr::TfToken("vertex"));

    usdMesh.CreateSubdivisionSchemeAttr(pxr::VtValue(pxr::TfToken("none")));

    const auto &meshMin = ignMesh->Min();
    const auto &meshMax = ignMesh->Max();
    pxr::VtArray<pxr::GfVec3f> extentBounds;
    extentBounds.push_back(pxr::GfVec3f(meshMin.X(), meshMin.Y(), meshMin.Z()));
    extentBounds.push_back(pxr::GfVec3f(meshMax.X(), meshMax.Y(), meshMax.Z()));
    usdMesh.CreateExtentAttr().Set(extentBounds);

    // TODO (ahcorde): Material inside the submesh
    int materialIndex = subMesh->MaterialIndex();
    if (materialIndex != -1)
    {
      auto material = ignMesh->MaterialByIndex(materialIndex);
      pxr::SdfPath materialPath;
      sdf::Material materialSdf = convert(material.get());
      if (!ParseSdfMaterial(&materialSdf, _stage, materialPath)) {
        gzerr << "Failed to parse sdf material" << std::endl;
        return false;
      }

      auto materialPrim = _stage->GetPrimAtPath(materialPath);
      if (!materialPrim)
      {
        gzerr << "Unable to get material prim at path ["
              + materialPath.GetString()
              + "], but a prim should exist at this path." << std::endl;
        return false;
      }

      auto materialUSD = pxr::UsdShadeMaterial(materialPrim);

      if(materialSdf.Emissive() != gz::math::Color(0, 0, 0, 1)
          || materialSdf.Specular() != gz::math::Color(0, 0, 0, 1)
          || materialSdf.PbrMaterial())
      {
        if (materialUSD)
        {
          pxr::UsdShadeMaterialBindingAPI(usdMesh).Bind(materialUSD);
        }
      }
    }

    pxr::UsdGeomXformCommonAPI meshXformAPI(usdMesh);

    meshXformAPI.SetScale(pxr::GfVec3f(
        _meshMsg.scale().x(), _meshMsg.scale().y(), _meshMsg.scale().z()));
  }

  return true;
}
}  // namespace gz::omniverse
