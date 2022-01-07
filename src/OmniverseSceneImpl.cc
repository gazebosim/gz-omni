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

#include <pxr/usd/sdf/copyUtils.h>

#include <ignition/common/Console.hh>

#include "OmniverseCamera.hh"
#include "OmniverseCapsule.hh"
#include "OmniverseGeometry.hh"
#include "OmniverseMaterial.hh"
#include "OmniverseRayQuery.hh"
#include "OmniverseRenderTarget.hh"
#include "OmniverseVisual.hh"

namespace ignition::rendering::omni {

OmniverseSceneImpl::SharedPtr OmniverseSceneImpl::Make(
    unsigned int _id, const std::string &_name, RenderEngine *_engine,
    pxr::UsdStageRefPtr _stage) {
  auto sp =
      std::shared_ptr<OmniverseSceneImpl>(new OmniverseSceneImpl(_id, _name));
  sp->engine = _engine;
  // TODO: In memory stage should not be a thing as there should be no valid
  // use case, we are doing it now because the nvidia omniverse client library
  // is not working atm.
  if (!_stage) {
    ignwarn << "Using in memory stage" << std::endl;
    sp->stage = pxr::UsdStage::CreateInMemory(_name);
  } else {
    sp->stage = _stage;
  }
  sp->stage->CreateClassPrim(pxr::SdfPath("/_nodes"));
  auto rootVisual =
      std::dynamic_pointer_cast<OmniverseVisual>(sp->CreateVisual());
  auto prim = sp->stage->DefinePrim(
      pxr::SdfPath("/" + NameToSdfPath(rootVisual->Name())));
  prim.GetReferences().AddInternalReference(rootVisual->Gprim().GetPath());
  sp->rootVisual = rootVisual;
  return sp;
}

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
  return OmniverseVisual::Make(_id, _name, this->SharedFromThis());
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
                                 OmniverseGeometry::GeometryType::Box);
}

GeometryPtr OmniverseSceneImpl::CreateConeImpl(unsigned int _id,
                                               const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 OmniverseGeometry::GeometryType::Cone);
}

GeometryPtr OmniverseSceneImpl::CreateCylinderImpl(unsigned int _id,
                                                   const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 OmniverseGeometry::GeometryType::Cylinder);
}

GeometryPtr OmniverseSceneImpl::CreatePlaneImpl(unsigned int _id,
                                                const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 OmniverseGeometry::GeometryType::Plane);
}

GeometryPtr OmniverseSceneImpl::CreateSphereImpl(unsigned int _id,
                                                 const std::string &_name) {
  return OmniverseGeometry::Make(_id, _name, this->SharedFromThis(),
                                 OmniverseGeometry::GeometryType::Sphere);
}

MeshPtr OmniverseSceneImpl::CreateMeshImpl(unsigned int _id,
                                           const std::string &_name,
                                           const MeshDescriptor &_desc) {
  return nullptr;
}

CapsulePtr OmniverseSceneImpl::CreateCapsuleImpl(unsigned int _id,
                                                 const std::string &_name) {
  return OmniverseCapsule::Make(_id, _name, this->SharedFromThis());
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

bool OmniverseSceneImpl::LoadImpl() { return true; }

bool OmniverseSceneImpl::InitImpl() { return true; }

}  // namespace ignition::rendering::omni
