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

#include "OmniverseScene.hh"

#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/sphere.h>

#include <ignition/common/Console.hh>

#include "OmniverseCapsule.hh"
#include "OmniverseGeometry.hh"

namespace ignition::rendering::omni {
OmniverseScene::OmniverseScene(RenderEngine *engine, unsigned int id,
                               std::string name)
    : BaseScene(id, name), _engine(engine) {
  // TODO: create stage in omniverse
  this->_stage = pxr::UsdStage::CreateInMemory(name);
}

VisualPtr OmniverseScene::RootVisual() const {
  // TODO: implement
  return nullptr;
}

math::Color OmniverseScene::AmbientLight() const {
  // TODO: implement
  return math::Color();
}

void SetAmbientLight(const math::Color &_color) {
  // TODO: implement
}

COMVisualPtr OmniverseScene::OmniverseScene::CreateCOMVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

InertiaVisualPtr OmniverseScene::CreateInertiaVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

JointVisualPtr OmniverseScene::CreateJointVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  // TODO: implement
  return nullptr;
}

LightVisualPtr OmniverseScene::CreateLightVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  // TODO: implement
  return nullptr;
}

DirectionalLightPtr OmniverseScene::CreateDirectionalLightImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

PointLightPtr OmniverseScene::CreatePointLightImpl(unsigned int _id,
                                                   const std::string &_name) {
  // TODO: implement
  return nullptr;
}

SpotLightPtr OmniverseScene::CreateSpotLightImpl(unsigned int _id,
                                                 const std::string &_name) {
  // TODO: implement
  return nullptr;
}

CameraPtr OmniverseScene::CreateCameraImpl(unsigned int _id,
                                           const std::string &_name) {
  // TODO: implement
  return nullptr;
}

DepthCameraPtr OmniverseScene::CreateDepthCameraImpl(unsigned int _id,
                                                     const std::string &_name) {
  // TODO: implement
  return nullptr;
}

VisualPtr OmniverseScene::CreateVisualImpl(unsigned int _id,
                                           const std::string &_name) {
  // TODO: implement
  return nullptr;
}

ArrowVisualPtr OmniverseScene::CreateArrowVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  // TODO: implement
  return nullptr;
}

AxisVisualPtr OmniverseScene::CreateAxisVisualImpl(unsigned int _id,
                                                   const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseScene::CreateBoxImpl(unsigned int _id,
                                          const std::string &_name) {
  auto cube = pxr::UsdGeomCube::Define(this->_stage, pxr::SdfPath(_name));
  return OmniverseGeometry::Make(this->shared_from_this(), cube);
}

GeometryPtr OmniverseScene::CreateCylinderImpl(unsigned int _id,
                                               const std::string &_name) {
  auto cylinder =
      pxr::UsdGeomCylinder::Define(this->_stage, pxr::SdfPath(_name));
  return OmniverseGeometry::Make(this->shared_from_this(), cylinder);
}

GeometryPtr OmniverseScene::CreatePlaneImpl(unsigned int _id,
                                            const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseScene::CreateSphereImpl(unsigned int _id,
                                             const std::string &_name) {
  auto sphere = pxr::UsdGeomSphere::Define(this->_stage, pxr::SdfPath(_name));
  return OmniverseGeometry::Make(this->shared_from_this(), sphere);
}

MeshPtr OmniverseScene::CreateMeshImpl(unsigned int _id,
                                       const std::string &_name,
                                       const MeshDescriptor &_desc) {
  return nullptr;
}

CapsulePtr OmniverseScene::CreateCapsuleImpl(unsigned int _id,
                                             const std::string &_name) {
  auto capsule = pxr::UsdGeomCapsule::Define(this->_stage, pxr::SdfPath(_name));
  return OmniverseCapsule::Make(this->shared_from_this(), capsule);
}

GridPtr OmniverseScene::CreateGridImpl(unsigned int _id,
                                       const std::string &_name) {
  // TODO: implement
  return nullptr;
}

MarkerPtr OmniverseScene::CreateMarkerImpl(unsigned int _id,
                                           const std::string &_name) {
  // TODO: implement
  return nullptr;
}

LidarVisualPtr OmniverseScene::CreateLidarVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  // TODO: implement
  return nullptr;
}

HeightmapPtr OmniverseScene::CreateHeightmapImpl(
    unsigned int _id, const std::string &_name,
    const HeightmapDescriptor &_desc) {
  // TODO: implement
  return nullptr;
}

WireBoxPtr OmniverseScene::CreateWireBoxImpl(unsigned int _id,
                                             const std::string &_name) {
  // TODO: implement
  return nullptr;
}

MaterialPtr OmniverseScene::CreateMaterialImpl(unsigned int _id,
                                               const std::string &_name) {
  // TODO: implement
  return nullptr;
}

RenderTexturePtr OmniverseScene::CreateRenderTextureImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

RenderWindowPtr OmniverseScene::CreateRenderWindowImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

RayQueryPtr OmniverseScene::CreateRayQueryImpl(unsigned int _id,
                                               const std::string &_name) {
  // TODO: implement
  return nullptr;
}

LightStorePtr OmniverseScene::Lights() const {
  // TODO: implement
  return nullptr;
}

SensorStorePtr OmniverseScene::Sensors() const {
  // TODO: implement
  return nullptr;
}

VisualStorePtr OmniverseScene::Visuals() const {
  // TODO: implement
  return nullptr;
}

MaterialMapPtr OmniverseScene::Materials() const {
  // TODO: implement
  return nullptr;
}

bool OmniverseScene::LoadImpl() { return true; }

bool OmniverseScene::InitImpl() { return true; }

}  // namespace ignition::rendering::omni
