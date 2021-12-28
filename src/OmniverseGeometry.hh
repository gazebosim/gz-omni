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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEGEOMETRY_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEGEOMETRY_HH

#include <pxr/usd/usdGeom/gprim.h>

#include <ignition/rendering/base/BaseGeometry.hh>
#include <ignition/rendering/base/BaseStorage.hh>

#include "OmniverseObject.hh"
#include "OmniverseScene.hh"

namespace ignition::rendering::omni {

class OmniverseGeometry : public BaseGeometry<OmniverseObject> {
 public:
  using Store = BaseGeometryStore<OmniverseGeometry>;
  using StorePtr = std::shared_ptr<Store>;
  using SharedPtr = std::shared_ptr<OmniverseGeometry>;

  enum class GeometryType { Box, Cone, Cylinder, Plane, Sphere, Mesh, Capsule };

  static std::string GeometryTypeToString(GeometryType _type) {
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

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        OmniverseScene::SharedPtr _scene, GeometryType _type) {
    auto sp = std::shared_ptr<OmniverseGeometry>(new OmniverseGeometry());
    sp->InitObject(_id, _name, _scene);
    sp->type = _type;
    return sp;
  }

  pxr::UsdGeomGprim Gprim() const { return this->gprim; }

  void SetGprim(pxr::UsdGeomGprim _gprim) { this->gprim = _gprim; }

  GeometryType Type() const { return this->type; }

  VisualPtr Parent() const override;

  void SetMaterial(MaterialPtr _material, bool _unique = true) override;

  bool HasParent() const override;

  MaterialPtr Material() const override;

 protected:
  pxr::UsdGeomGprim gprim;
  GeometryType type;
};

}  // namespace ignition::rendering::omni

#endif
