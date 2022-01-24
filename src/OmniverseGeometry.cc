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

#include "OmniverseGeometry.hh"

#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include "OmniverseVisual.hh"

namespace ignition::rendering::omni {

OmniverseGeometry::SharedPtr OmniverseGeometry::Make(
    unsigned int _id, const std::string& _name,
    OmniverseScene::SharedPtr _scene, GeometryType _type) {
  auto sp = std::shared_ptr<OmniverseGeometry>(new OmniverseGeometry());
  sp->Init(_id, _name, _scene);
  sp->type = _type;
  return sp;
}

std::string OmniverseGeometry::GeometryTypeToString(GeometryType _type) {
  switch (_type) {
    case GeometryType::Box:
      return "Box";
    case GeometryType::Cone:
      return "Cone";
    case GeometryType::Cylinder:
      return "Cylinder";
    case GeometryType::Plane:
      return "Plane";
    case GeometryType::Sphere:
      return "Sphere";
    case GeometryType::Mesh:
      return "Mesh";
    case GeometryType::Capsule:
      return "Capsule";
    default:
      return "Unknown";
  }
}

bool OmniverseGeometry::AttachToVisual(VisualPtr _visual) {
  auto ovVisual = std::dynamic_pointer_cast<OmniverseVisual>(_visual);
  if (!ovVisual) {
    ignerr << "Failed to attach to visual '" << _visual->Id()
           << "' (not an omniverse visual)" << std::endl;
    return false;
  }

  auto path = ovVisual->Xformable().GetPath().AppendPath(
      pxr::SdfPath(NameToSdfPath(this->Name())));
  switch (this->Type()) {
    case OmniverseGeometry::GeometryType::Box: {
      this->gprim = pxr::UsdGeomCube::Define(this->Stage(), path);
      // usd cube is 2x2 by default
      pxr::VtArray<pxr::GfVec3f> extent;
      extent.push_back(pxr::GfVec3f(-1.0f));
      extent.push_back(pxr::GfVec3f(1.0f));
      this->gprim.CreateExtentAttr(pxr::VtValue(extent));
      break;
    }
    // TODO: Support cone
    // case OmniverseGeometry::GeometryType::Cone:
    case OmniverseGeometry::GeometryType::Cylinder: {
      this->gprim = pxr::UsdGeomCylinder::Define(this->Stage(), path);
      pxr::VtArray<pxr::GfVec3f> extent;
      extent.push_back(pxr::GfVec3f(-1.0f));
      extent.push_back(pxr::GfVec3f(1.0f));
      this->gprim.CreateExtentAttr(pxr::VtValue(extent));
      break;
    }
    // TODO: Support plane
    // case OmniverseGeometry::GeometryType::Plane:
    case OmniverseGeometry::GeometryType::Sphere: {
      this->gprim = pxr::UsdGeomSphere::Define(this->Stage(), path);
      pxr::VtArray<pxr::GfVec3f> extent;
      extent.push_back(pxr::GfVec3f(-1.0f));
      extent.push_back(pxr::GfVec3f(1.0f));
      this->gprim.CreateExtentAttr(pxr::VtValue(extent));
      break;
    }
    case OmniverseGeometry::GeometryType::Capsule: {
      this->gprim = pxr::UsdGeomCapsule::Define(this->Stage(), path);
      pxr::VtArray<pxr::GfVec3f> extent;
      extent.push_back(pxr::GfVec3f(-0.5f, -0.5f, -1.0f));
      extent.push_back(pxr::GfVec3f(0.5f, 0.5f, 1.0f));
      this->gprim.CreateExtentAttr(pxr::VtValue(extent));
      break;
    }
    default:
      ignerr << "Failed to attach geometry (unsuported geometry type '"
             << this->GeometryTypeToString(this->Type()) << "')" << std::endl;
      return false;
  }
  this->parent = std::move(_visual);
  return true;
}

VisualPtr OmniverseGeometry::Parent() const { return this->parent; }

void OmniverseGeometry::SetMaterial(MaterialPtr _material, bool _unique) {
  // TODO: implement
}

bool OmniverseGeometry::HasParent() const {
  return static_cast<bool>(this->parent);
}

MaterialPtr OmniverseGeometry::Material() const {
  // TODO: implement
  return nullptr;
}

}  // namespace ignition::rendering::omni
