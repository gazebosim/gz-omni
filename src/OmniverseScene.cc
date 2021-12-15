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

#include <ignition/common/Console.hh>

namespace ignition::rendering::omni {
OmniverseScene::OmniverseScene(RenderEngine *engine, unsigned int id,
                               std::string name)
    : BaseScene(id, name), _engine(engine) {
  // TODO: create stage in omniverse
  this->_stage = pxr::UsdStage::CreateInMemory(name);
}

VisualPtr OmniverseScene::RootVisual() const {
  throw std::runtime_error("not implemented");
}

math::Color OmniverseScene::AmbientLight() const {
  throw std::runtime_error("not implemented");
}

void SetAmbientLight(const math::Color &_color) {
  throw std::runtime_error("not implemented");
}

COMVisualPtr OmniverseScene::OmniverseScene::CreateCOMVisualImpl(
    unsigned int _id, const std::string &_name) {
  throw std::runtime_error("not implemented");
}

InertiaVisualPtr OmniverseScene::CreateInertiaVisualImpl(
    unsigned int _id, const std::string &_name) {
  throw std::runtime_error("not implemented");
}

JointVisualPtr OmniverseScene::CreateJointVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  throw std::runtime_error("not implemented");
}

LightVisualPtr OmniverseScene::CreateLightVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  throw std::runtime_error("not implemented");
}

DirectionalLightPtr OmniverseScene::CreateDirectionalLightImpl(
    unsigned int _id, const std::string &_name) {
  throw std::runtime_error("not implemented");
}

PointLightPtr OmniverseScene::CreatePointLightImpl(unsigned int _id,
                                                   const std::string &_name) {
  throw std::runtime_error("not implemented");
}

SpotLightPtr OmniverseScene::CreateSpotLightImpl(unsigned int _id,
                                                 const std::string &_name) {
  throw std::runtime_error("not implemented");
}

CameraPtr OmniverseScene::CreateCameraImpl(unsigned int _id,
                                           const std::string &_name) {
  throw std::runtime_error("not implemented");
}

DepthCameraPtr OmniverseScene::CreateDepthCameraImpl(unsigned int _id,
                                                     const std::string &_name) {
  throw std::runtime_error("not implemented");
}

VisualPtr OmniverseScene::CreateVisualImpl(unsigned int _id,
                                           const std::string &_name) {
  throw std::runtime_error("not implemented");
}

ArrowVisualPtr OmniverseScene::CreateArrowVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  throw std::runtime_error("not implemented");
}

AxisVisualPtr OmniverseScene::CreateAxisVisualImpl(unsigned int _id,
                                                   const std::string &_name) {
  throw std::runtime_error("not implemented");
}

GeometryPtr OmniverseScene::CreateBoxImpl(unsigned int _id,
                                          const std::string &_name) {
  throw std::runtime_error("not implemented");
}

GeometryPtr OmniverseScene::CreateConeImpl(unsigned int _id,
                                           const std::string &_name) {
  throw std::runtime_error("not implemented");
}

GeometryPtr OmniverseScene::CreateCylinderImpl(unsigned int _id,
                                               const std::string &_name) {
  throw std::runtime_error("not implemented");
}

GeometryPtr OmniverseScene::CreatePlaneImpl(unsigned int _id,
                                            const std::string &_name) {
  throw std::runtime_error("not implemented");
}

GeometryPtr OmniverseScene::CreateSphereImpl(unsigned int _id,
                                             const std::string &_name) {
  throw std::runtime_error("not implemented");
}

MeshPtr OmniverseScene::CreateMeshImpl(unsigned int _id,
                                       const std::string &_name,
                                       const MeshDescriptor &_desc) {
  throw std::runtime_error("not implemented");
}

CapsulePtr OmniverseScene::CreateCapsuleImpl(unsigned int _id,
                                             const std::string &_name) {
  throw std::runtime_error("not implemented");
}

GridPtr OmniverseScene::CreateGridImpl(unsigned int _id,
                                       const std::string &_name) {
  throw std::runtime_error("not implemented");
}

MarkerPtr OmniverseScene::CreateMarkerImpl(unsigned int _id,
                                           const std::string &_name) {
  throw std::runtime_error("not implemented");
}

LidarVisualPtr OmniverseScene::CreateLidarVisualImpl(unsigned int _id,
                                                     const std::string &_name) {
  throw std::runtime_error("not implemented");
}

HeightmapPtr OmniverseScene::CreateHeightmapImpl(
    unsigned int _id, const std::string &_name,
    const HeightmapDescriptor &_desc) {
  throw std::runtime_error("not implemented");
}

WireBoxPtr OmniverseScene::CreateWireBoxImpl(unsigned int _id,
                                             const std::string &_name) {
  throw std::runtime_error("not implemented");
}

MaterialPtr OmniverseScene::CreateMaterialImpl(unsigned int _id,
                                               const std::string &_name) {
  throw std::runtime_error("not implemented");
}

RenderTexturePtr OmniverseScene::CreateRenderTextureImpl(
    unsigned int _id, const std::string &_name) {
  throw std::runtime_error("not implemented");
}

RenderWindowPtr OmniverseScene::CreateRenderWindowImpl(
    unsigned int _id, const std::string &_name) {
  throw std::runtime_error("not implemented");
}

RayQueryPtr OmniverseScene::CreateRayQueryImpl(unsigned int _id,
                                               const std::string &_name) {
  throw std::runtime_error("not implemented");
}

LightStorePtr OmniverseScene::Lights() const {
  throw std::runtime_error("not implemented");
}

SensorStorePtr OmniverseScene::Sensors() const {
  throw std::runtime_error("not implemented");
}

VisualStorePtr OmniverseScene::Visuals() const {
  throw std::runtime_error("not implemented");
}

MaterialMapPtr OmniverseScene::Materials() const {
  throw std::runtime_error("not implemented");
}

bool OmniverseScene::LoadImpl() { throw std::runtime_error("not implemented"); }

bool OmniverseScene::InitImpl() { throw std::runtime_error("not implemented"); }
}  // namespace ignition::rendering::omni
