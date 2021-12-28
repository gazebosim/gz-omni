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

#include "OmniverseVisual.hh"

#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/sphere.h>

#include "Utils.hh"

namespace ignition::rendering::omni {

bool OmniverseVisual::AttachGeometry(GeometryPtr _geometry) {
  auto ovgeom = std::dynamic_pointer_cast<OmniverseGeometry>(_geometry);
  if (!ovgeom) {
    ignerr << "Failed to attach geometry '" << _geometry->Id()
           << "' (not an omniverse geometry)" << std::endl;
    return false;
  }
  if (ovgeom->Gprim().GetPrim().IsValid()) {
    ignwarn << "Geometry already has a prim associated, overriding it"
            << std::endl;
  }

  auto path = this->Prim().GetPrimPath().AppendPath(
      pxr::SdfPath(NameToSdfPath(_geometry->Name())));
  switch (ovgeom->Type()) {
    case OmniverseGeometry::GeometryType::Box:
      ovgeom->SetGprim(pxr::UsdGeomCube::Define(this->Stage(), path));
      break;
    case OmniverseGeometry::GeometryType::Cylinder:
      ovgeom->SetGprim(pxr::UsdGeomCylinder::Define(this->Stage(), path));
      break;
    case OmniverseGeometry::GeometryType::Sphere:
      ovgeom->SetGprim(pxr::UsdGeomSphere::Define(this->Stage(), path));
      break;
    case OmniverseGeometry::GeometryType::Capsule:
      ovgeom->SetGprim(pxr::UsdGeomCapsule::Define(this->Stage(), path));
      break;
    default:
      ignerr << "Failed to attach geometry (unsuported geometry type '"
             << OmniverseGeometry::GeometryTypeToString(ovgeom->Type()) << "')"
             << std::endl;
      return false;
  }

  return true;
}

bool OmniverseVisual::DetachGeometry(GeometryPtr _geometry) { return false; }

}  // namespace ignition::rendering::omni
