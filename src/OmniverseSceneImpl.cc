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

#include "OmniverseSceneImpl.hh"

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
#include "OmniverseRayQuery.hh"
#include "OmniverseRenderTarget.hh"
#include "OmniverseVisual.hh"

namespace ignition::rendering::omni {

VisualPtr OmniverseSceneImpl::RootVisual() const { return this->rootVisual; }

math::Color OmniverseSceneImpl::AmbientLight() const {
  // TODO: implement
  return math::Color();
}

void OmniverseSceneImpl::SetAmbientLight(const math::Color &_color) {
  // TODO: implement
}

COMVisualPtr OmniverseSceneImpl::OmniverseSceneImpl::CreateCOMVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

InertiaVisualPtr OmniverseSceneImpl::CreateInertiaVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

JointVisualPtr OmniverseSceneImpl::CreateJointVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

LightVisualPtr OmniverseSceneImpl::CreateLightVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

DirectionalLightPtr OmniverseSceneImpl::CreateDirectionalLightImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

PointLightPtr OmniverseSceneImpl::CreatePointLightImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

SpotLightPtr OmniverseSceneImpl::CreateSpotLightImpl(unsigned int _id,
                                                     const std::string &_name) {
  // TODO: implement
  return nullptr;
}

CameraPtr OmniverseSceneImpl::CreateCameraImpl(unsigned int _id,
                                               const std::string &_name) {
  return OmniverseCamera::Make(_id, _name, this->SharedFromThis());
}

DepthCameraPtr OmniverseSceneImpl::CreateDepthCameraImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

VisualPtr OmniverseSceneImpl::CreateVisualImpl(unsigned int _id,
                                               const std::string &_name) {
  // TODO: implement
  return nullptr;
}

ArrowVisualPtr OmniverseSceneImpl::CreateArrowVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

AxisVisualPtr OmniverseSceneImpl::CreateAxisVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseSceneImpl::CreateBoxImpl(unsigned int _id,
                                              const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 pxr::UsdGeomCube());
}

GeometryPtr OmniverseSceneImpl::CreateConeImpl(unsigned int _id,
                                               const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseSceneImpl::CreateCylinderImpl(unsigned int _id,
                                                   const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 pxr::UsdGeomCylinder());
}

GeometryPtr OmniverseSceneImpl::CreatePlaneImpl(unsigned int _id,
                                                const std::string &_name) {
  // TODO: implement
  return nullptr;
}

GeometryPtr OmniverseSceneImpl::CreateSphereImpl(unsigned int _id,
                                                 const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 pxr::UsdGeomSphere());
}

MeshPtr OmniverseSceneImpl::CreateMeshImpl(unsigned int _id,
                                           const std::string &_name,
                                           const MeshDescriptor &_desc) {
  return nullptr;
}

CapsulePtr OmniverseSceneImpl::CreateCapsuleImpl(unsigned int _id,
                                                 const std::string &_name) {
  return OmniverseCapsule::Make(_id, _name, this->SharedFromThis(),
                                pxr::UsdGeomCapsule());
}

GridPtr OmniverseSceneImpl::CreateGridImpl(unsigned int _id,
                                           const std::string &_name) {
  // TODO: implement
  return nullptr;
}

MarkerPtr OmniverseSceneImpl::CreateMarkerImpl(unsigned int _id,
                                               const std::string &_name) {
  // TODO: implement
  return nullptr;
}

LidarVisualPtr OmniverseSceneImpl::CreateLidarVisualImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

HeightmapPtr OmniverseSceneImpl::CreateHeightmapImpl(
    unsigned int _id, const std::string &_name,
    const HeightmapDescriptor &_desc) {
  // TODO: implement
  return nullptr;
}

WireBoxPtr OmniverseSceneImpl::CreateWireBoxImpl(unsigned int _id,
                                                 const std::string &_name) {
  // TODO: implement
  return nullptr;
}

MaterialPtr OmniverseSceneImpl::CreateMaterialImpl(unsigned int _id,
                                                   const std::string &_name) {
  return OmniverseMaterial::Make(_id, _name, this->SharedFromThis());
}

RenderTexturePtr OmniverseSceneImpl::CreateRenderTextureImpl(
    unsigned int _id, const std::string &_name) {
  return OmniverseRenderTexture::Make(_id, _name, this->SharedFromThis());
}

RenderWindowPtr OmniverseSceneImpl::CreateRenderWindowImpl(
    unsigned int _id, const std::string &_name) {
  // TODO: implement
  return nullptr;
}

RayQueryPtr OmniverseSceneImpl::CreateRayQueryImpl(unsigned int _id,
                                                   const std::string &_name) {
  return OmniverseRayQuery::Make(_id, _name, this->SharedFromThis());
}

LightStorePtr OmniverseSceneImpl::Lights() const { return this->lightStore; }

SensorStorePtr OmniverseSceneImpl::Sensors() const { return this->sensorStore; }

VisualStorePtr OmniverseSceneImpl::Visuals() const { return this->visualStore; }

MaterialMapPtr OmniverseSceneImpl::Materials() const {
  return this->materialMap;
}

bool OmniverseSceneImpl::LoadImpl() {
  // TODO: create stage in omniverse
  this->stage = pxr::UsdStage::CreateInMemory(name);
  return true;
}

bool OmniverseSceneImpl::InitImpl() { return true; }

}  // namespace ignition::rendering::omni
