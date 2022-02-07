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

#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

namespace ignition
{
namespace omniverse
{
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
    default:
      std::cerr << "Failed to attach geometry (unsuported geometry type '"
                << this->GeometryTypeToString(this->Type()) << "')"
                << std::endl;
      return false;
  }
  return true;
}
}  // namespace omniverse
}  // namespace ignition
