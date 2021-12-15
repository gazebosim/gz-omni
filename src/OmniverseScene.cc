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

#include "OmniverseCamera.hh"
#include "OmniverseCapsule.hh"
#include "OmniverseGeometry.hh"
#include "OmniverseMaterial.hh"
#include "OmniverseVisual.hh"

namespace ignition::rendering::omni {
OmniverseScene::OmniverseScene(RenderEngine *engine, unsigned int id,
                               std::string name)
    : BaseScene(id, name), _engine(engine) {
  // TODO: create stage in omniverse
  this->_stage = pxr::UsdStage::CreateInMemory(name);
}

VisualPtr OmniverseScene::RootVisual() const { return OmniverseVisual::Make(); }

math::Color OmniverseScene::AmbientLight() const {
  // TODO: implement
  return math::Color();
}

void OmniverseScene::SetAmbientLight(const math::Color &_color) {
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
  return OmniverseCamera::Make();
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
  return OmniverseGeometry::Make(this->shared_from_this(), pxr::UsdGeomCube());
}

GeometryPtr OmniverseScene::CreateConeImpl(unsigned int _id,
                                           const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseScene::CreateCylinderImpl(unsigned int _id,
                                               const std::string &_name) {
  return OmniverseGeometry::Make(this->shared_from_this(),
                                 pxr::UsdGeomCylinder());
}

GeometryPtr OmniverseScene::CreatePlaneImpl(unsigned int _id,
                                            const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseScene::CreateSphereImpl(unsigned int _id,
                                             const std::string &_name) {
  return OmniverseGeometry::Make(this->shared_from_this(),
                                 pxr::UsdGeomSphere());
}

MeshPtr OmniverseScene::CreateMeshImpl(unsigned int _id,
                                       const std::string &_name,
                                       const MeshDescriptor &_desc) {
  return nullptr;
}

CapsulePtr OmniverseScene::CreateCapsuleImpl(unsigned int _id,
                                             const std::string &_name) {
  return OmniverseCapsule::Make(this->shared_from_this(),
                                pxr::UsdGeomCapsule());
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
  return OmniverseMaterial::Make();
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
  return this->_lightStore;
}

SensorStorePtr OmniverseScene::Sensors() const {
  return this->_sensorStore;
}

VisualStorePtr OmniverseScene::Visuals() const {
  return this->_visualStore;
}

MaterialMapPtr OmniverseScene::Materials() const { return this->_materialMap; }

bool OmniverseScene::LoadImpl() { return true; }

bool OmniverseScene::InitImpl() { return true; }

}  // namespace ignition::rendering::omni
