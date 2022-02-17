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

#include "IgnitionGeometry.hpp"

#include <ignition/common/Console.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/common/Mesh.hh>
#include <ignition/common/SubMesh.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/common/URI.hh>
#include <ignition/common/Util.hh>

#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>


namespace ignition
{
namespace omniverse
{
  bool endsWith(const std::string_view &str, const std::string_view &suffix)
  {
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
  }

  inline std::string removeDash(const std::string &_str)
  {
    std::string result = _str;
    std::replace(result.begin(), result.end(), '-', '_');
    return result;
  }

IgnitionGeometry::SharedPtr IgnitionGeometry::Make(
    unsigned int _id, const std::string &_name, Scene::SharedPtr &_scene,
    const ignition::msgs::Geometry::Type &_type)
{
  auto sp = std::shared_ptr<IgnitionGeometry>(new IgnitionGeometry());
  sp->id = _id;
  sp->name = _name;
  sp->type = _type;
  sp->scene = _scene;
  return sp;
}

std::string IgnitionGeometry::GeometryTypeToString(
    const ignition::msgs::Geometry::Type &_type)
{
  switch (_type)
  {
    case ignition::msgs::Geometry::BOX:
      return "Box";
    case ignition::msgs::Geometry::CONE:
      return "Cone";
    case ignition::msgs::Geometry::CYLINDER:
      return "Cylinder";
    case ignition::msgs::Geometry::PLANE:
      return "Plane";
    case ignition::msgs::Geometry::SPHERE:
      return "Sphere";
    case ignition::msgs::Geometry::MESH:
      return "Mesh";
    case ignition::msgs::Geometry::CAPSULE:
      return "Capsule";
    default:
      return "Unknown";
  }
}

ignition::msgs::Geometry::Type IgnitionGeometry::Type() const
{
  return this->type;
}

bool IgnitionGeometry::AttachToVisual(const ignition::msgs::Geometry &_geom,
                                      const std::string &_path,
                                      const pxr::UsdShadeMaterial &_material)
{
  // auto path = ovVisual->Xformable().GetPath().AppendPath(
  //     pxr::SdfPath(NameToSdfPath(this->Name())));
  std::string sdfGeometryPath = _path + "/geometry";

  switch (this->Type())
  {
    case ignition::msgs::Geometry::BOX:
    {
      auto usdCube = this->scene->CreateCube(sdfGeometryPath);
      this->gprim = pxr::UsdGeomGprim(usdCube);
      usdCube.CreateSizeAttr().Set(1.0);
      pxr::GfVec3f endPoint(0.5);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCube.CreateExtentAttr().Set(extentBounds);
      pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
      cubeXformAPI.SetScale(pxr::GfVec3f(_geom.box().size().x(),
                                         _geom.box().size().y(),
                                         _geom.box().size().z()));
      pxr::UsdShadeMaterialBindingAPI(usdCube).Bind(_material);
      break;
    }
    // TODO: Support cone
    // case ignition::msgs::Geometry::CONE:
    case ignition::msgs::Geometry::CYLINDER:
    {
      auto usdCylinder = this->scene->CreateCylinder(sdfGeometryPath);
      this->gprim = pxr::UsdGeomGprim(usdCylinder);
      double radius = _geom.cylinder().radius();
      double length = _geom.cylinder().length();

      usdCylinder.CreateRadiusAttr().Set(radius);
      usdCylinder.CreateHeightAttr().Set(length);
      pxr::GfVec3f endPoint(radius);
      endPoint[2] = length * 0.5;
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCylinder.CreateExtentAttr().Set(extentBounds);
      pxr::UsdShadeMaterialBindingAPI(usdCylinder).Bind(_material);
      break;
    }
    case ignition::msgs::Geometry::PLANE:
    {
      auto usdCube = this->scene->CreateCube(sdfGeometryPath);
      this->gprim = pxr::UsdGeomGprim(usdCube);
      usdCube.CreateSizeAttr().Set(1.0);
      pxr::GfVec3f endPoint(0.5);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCube.CreateExtentAttr().Set(extentBounds);

      pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
      cubeXformAPI.SetScale(pxr::GfVec3f(_geom.plane().size().x(),
                                         _geom.plane().size().y(), 0.25));
      pxr::UsdShadeMaterialBindingAPI(usdCube).Bind(_material);
      break;
    }
    case ignition::msgs::Geometry::ELLIPSOID:
    {
      auto usdEllipsoid = this->scene->CreateSphere(sdfGeometryPath);
      this->gprim = pxr::UsdGeomGprim(usdEllipsoid);
      const auto maxRadii =
          ignition::math::Vector3d(_geom.ellipsoid().radii().x(),
                                   _geom.ellipsoid().radii().y(),
                                   _geom.ellipsoid().radii().z())
              .Max();
      usdEllipsoid.CreateRadiusAttr().Set(0.5);
      pxr::UsdGeomXformCommonAPI xform(usdEllipsoid);
      xform.SetScale(pxr::GfVec3f{
          static_cast<float>(_geom.ellipsoid().radii().x() / maxRadii),
          static_cast<float>(_geom.ellipsoid().radii().y() / maxRadii),
          static_cast<float>(_geom.ellipsoid().radii().z() / maxRadii),
      });
      // extents is the bounds before any transformation
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(pxr::GfVec3f{static_cast<float>(-maxRadii)});
      extentBounds.push_back(pxr::GfVec3f{static_cast<float>(maxRadii)});
      usdEllipsoid.CreateExtentAttr().Set(extentBounds);
      pxr::UsdShadeMaterialBindingAPI(usdEllipsoid).Bind(_material);
      break;
    }
    case ignition::msgs::Geometry::SPHERE:
    {
      auto usdSphere = this->scene->CreateSphere(sdfGeometryPath);
      this->gprim = pxr::UsdGeomGprim(usdSphere);
      double radius = _geom.sphere().radius();
      usdSphere.CreateRadiusAttr().Set(radius);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(pxr::GfVec3f(-1.0 * radius));
      extentBounds.push_back(pxr::GfVec3f(radius));
      usdSphere.CreateExtentAttr().Set(extentBounds);
      pxr::UsdShadeMaterialBindingAPI(usdSphere).Bind(_material);
      break;
    }
    case ignition::msgs::Geometry::CAPSULE:
    {
      auto usdCapsule = this->scene->CreateCapsule(sdfGeometryPath);
      this->gprim = pxr::UsdGeomGprim(usdCapsule);
      double radius = _geom.capsule().radius();
      double length = _geom.capsule().length();
      usdCapsule.CreateRadiusAttr().Set(radius);
      usdCapsule.CreateHeightAttr().Set(length);
      pxr::GfVec3f endPoint(radius);
      endPoint[2] += 0.5 * length;
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCapsule.CreateExtentAttr().Set(extentBounds);
      pxr::UsdShadeMaterialBindingAPI(usdCapsule).Bind(_material);
      break;
    }
    // TODO: This can probably use code from sdformat
    case ignition::msgs::Geometry::MESH:
    {
      ignition::common::URI uri(_geom.mesh().filename());
      std::string fullname;

      if (uri.Scheme() == "https" || uri.Scheme() == "http")
      {
        fullname =
          ignition::common::findFile(uri.Str());
      }
      else
      {
        fullname =
          ignition::common::findFile(_geom.mesh().filename());
      }

      auto ignMesh = ignition::common::MeshManager::Instance()->Load(
          fullname);

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
          std::cerr << "Unable to get a shared pointer to submesh at index ["
                    << i << "] of parent mesh [" << ignMesh->Name() << "]\n";
          return false;
        }
        if (isUSDPathInSubMeshName)
        {
          if (ignMesh->SubMeshCount() != 1)
          {
            std::string pathLowerCase = ignition::common::lowercase(_path);
            std::string subMeshLowerCase = ignition::common::lowercase(subMesh->Name());

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
            std::cerr << "Submesh " << subMesh->Name()
                      << " has a primitive type that is not supported.\n";
            return false;
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

        auto usdMesh = this->scene->CreateMesh(primName);
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

        pxr::UsdGeomXformCommonAPI meshXformAPI(usdMesh);

        meshXformAPI.SetScale(
          pxr::GfVec3f(_geom.mesh().scale().x(),
                       _geom.mesh().scale().y(),
                       _geom.mesh().scale().z()));
        pxr::UsdShadeMaterialBindingAPI(usdMesh).Bind(_material);
      }
      break;
    }
    default:
      ignerr << "Failed to attach geometry (unsuported geometry type '"
             << this->GeometryTypeToString(this->Type()) << "')" << std::endl;
      return false;
  }
  return true;
}
}  // namespace omniverse
}  // namespace ignition
