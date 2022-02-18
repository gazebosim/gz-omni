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

#include <ignition/common/Console.hh>
#include <ignition/common/Mesh.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/common/SubMesh.hh>
#include <ignition/common/URI.hh>
#include <ignition/common/Util.hh>

#include <pxr/usd/usdGeom/xformCommonAPI.h>

namespace ignition::omniverse
{
bool endsWith(const std::string_view &str, const std::string_view &suffix)
{
  return str.size() >= suffix.size() &&
         0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

inline std::string removeDash(const std::string &_str)
{
  std::string result = _str;
  std::replace(result.begin(), result.end(), '-', '_');
  return result;
}

pxr::UsdGeomMesh UpdateMesh(const ignition::msgs::MeshGeom &_meshMsg,
                            const std::string &_path,
                            const pxr::UsdStageRefPtr &_stage)
{
  ignition::common::URI uri(_meshMsg.filename());
  std::string fullname;

  std::string home;
  if (!ignition::common::env("HOME", home, false))
  {
    std::cerr << "The HOME environment variable was not defined, "
              << "so the resource [" << fullname << "] could not be found\n";
    return pxr::UsdGeomMesh();
  }
  if (uri.Scheme() == "https" || uri.Scheme() == "http")
  {
    auto systemPaths = ignition::common::systemPaths();

    std::vector<std::string> tokens = ignition::common::split(uri.Path().Str(), "/");
    std::string server = tokens[0];
    std::string versionServer = tokens[1];
    std::string owner = ignition::common::lowercase(tokens[2]);
    std::string type = ignition::common::lowercase(tokens[3]);
    std::string modelName = ignition::common::lowercase(tokens[4]);
    std::string modelVersion = ignition::common::lowercase(tokens[5]);

    fullname = ignition::common::joinPaths(
      home, ".ignition", "fuel", server, owner, type, modelName, modelVersion);
    systemPaths->AddFilePaths(fullname);

    for (int i = 7; i < tokens.size(); i++)
    {
      fullname = ignition::common::joinPaths(
        fullname, ignition::common::lowercase(tokens[i]));
      systemPaths->AddFilePaths(fullname);
    }
  }
  else
  {
    fullname = ignition::common::findFile(_meshMsg.filename());
  }

  auto ignMesh = ignition::common::MeshManager::Instance()->Load(fullname);

  // Some Meshes are splited in some submeshes, this loop check if the name
  // of the path is the same as the name of the submesh. In this case
  // we create a USD mesh per submesh.
  bool isUSDPathInSubMeshName = false;
  for (unsigned int i = 0; i < ignMesh->SubMeshCount(); ++i)
  {
    auto subMesh = ignMesh->SubMeshByIndex(i).lock();

    if (ignMesh->SubMeshCount() != 1)
    {
      std::string pathLowerCase = ignition::common::lowercase(_path);
      std::string subMeshLowerCase =
          ignition::common::lowercase(subMesh->Name());

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
      return pxr::UsdGeomMesh();
    }
    if (isUSDPathInSubMeshName)
    {
      if (ignMesh->SubMeshCount() != 1)
      {
        std::string pathLowerCase = ignition::common::lowercase(_path);
        std::string subMeshLowerCase =
            ignition::common::lowercase(subMesh->Name());

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
      case ignition::common::SubMesh::PrimitiveType::POINTS:
        verticesPerFace = 1;
        numFaces = subMesh->IndexCount();
        break;
      case ignition::common::SubMesh::PrimitiveType::LINES:
        verticesPerFace = 2;
        numFaces = subMesh->IndexCount() / 2;
        break;
      case ignition::common::SubMesh::PrimitiveType::TRIANGLES:
        verticesPerFace = 3;
        numFaces = subMesh->IndexCount() / 3;
        break;
      case ignition::common::SubMesh::PrimitiveType::LINESTRIPS:
      case ignition::common::SubMesh::PrimitiveType::TRIFANS:
      case ignition::common::SubMesh::PrimitiveType::TRISTRIPS:
      default:
        ignerr << "Submesh " << subMesh->Name()
               << " has a primitive type that is not supported." << std::endl;
        return pxr::UsdGeomMesh();
    }
    // TODO(adlarkin) update this loop to allow for varying element
    // values in the array (see TODO note above). Right now, the
    // array only allows for all elements to have one value, which in
    // this case is "verticesPerFace"
    for (unsigned int n = 0; n < numFaces; ++n)
      faceVertexCounts.push_back(verticesPerFace);

    std::string primName = _path + "/" + subMesh->Name();
    primName = removeDash(primName);

    if (endsWith(primName, "/"))
    {
      primName.erase(primName.size() - 1);
    }

    auto usdMesh = pxr::UsdGeomMesh::Define(_stage, pxr::SdfPath(_path));
    usdMesh.CreatePointsAttr().Set(meshPoints);
    usdMesh.CreateFaceVertexIndicesAttr().Set(faceVertexIndices);
    usdMesh.CreateFaceVertexCountsAttr().Set(faceVertexCounts);

    auto coordinates = usdMesh.CreatePrimvar(
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
      // sdf::Material materialSdf = sdf::usd::convert(material);
      // auto materialUSD = ParseSdfMaterial(&materialSdf, _stage);

      // if(materialSdf.Emissive() != ignition::math::Color(0, 0, 0, 1)
      //     || materialSdf.Specular() != ignition::math::Color(0, 0, 0, 1)
      //     || materialSdf.PbrMaterial())
      // {
      //   if (materialUSD)
      //   {
      //     pxr::UsdShadeMaterialBindingAPI(usdMesh).Bind(materialUSD);
      //   }
      // }
    }

    pxr::UsdGeomXformCommonAPI meshXformAPI(usdMesh);

    meshXformAPI.SetScale(pxr::GfVec3f(
        _meshMsg.scale().x(), _meshMsg.scale().y(), _meshMsg.scale().z()));
    return usdMesh;
  }
  return pxr::UsdGeomMesh();
}
}  // namespace ignition::omniverse
