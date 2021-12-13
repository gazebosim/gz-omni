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

#include <exception>
#include "OmniverseScene.hh"

namespace ignition::rendering::omni
{
  void OmniverseScene::Load() { throw std::runtime_error("not implemented"); }
  void OmniverseScene::Init() { throw std::runtime_error("not implemented"); }
  void OmniverseScene::Fini() { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::IsInitialized() const { throw std::runtime_error("not implemented"); }
  unsigned int OmniverseScene::Id() const { throw std::runtime_error("not implemented"); }
  std::string OmniverseScene::Name() const { throw std::runtime_error("not implemented"); }
  RenderEngine *OmniverseScene::Engine() const { throw std::runtime_error("not implemented"); }
  common::Time IGN_DEPRECATED(4) OmniverseScene::SimTime() const { throw std::runtime_error("not implemented"); }
  std::chrono::steady_clock::duration OmniverseScene::Time() const { throw std::runtime_error("not implemented"); }
  void IGN_DEPRECATED(4) OmniverseScene::SetSimTime(const common::Time &_time) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetTime(const std::chrono::steady_clock::duration &_time) { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::RootVisual() const { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::VisualAt(const CameraPtr &_camera, const math::Vector2i &_mousePos) { throw std::runtime_error("not implemented"); }
  math::Color OmniverseScene::AmbientLight() const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetAmbientLight(double _r, double _g, double _b, double _a = 1.0) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetAmbientLight(const math::Color &_color) { throw std::runtime_error("not implemented"); }
  math::Color OmniverseScene::BackgroundColor() const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetBackgroundColor(double _r, double _g, double _b, double _a = 1.0) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetBackgroundColor(const math::Color &_color) { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::IsGradientBackgroundColor() const { throw std::runtime_error("not implemented"); }
  std::array<math::Color, 4> OmniverseScene::GradientBackgroundColor() const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetGradientBackgroundColor(const std::array<math::Color, 4> &_colors) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::RemoveGradientBackgroundColor() { throw std::runtime_error("not implemented"); }
  MaterialPtr OmniverseScene::BackgroundMaterial() const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetBackgroundMaterial(MaterialPtr _material) { throw std::runtime_error("not implemented"); }
  unsigned int OmniverseScene::NodeCount() const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasNode(ConstNodePtr _node) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasNodeId(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasNodeName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  NodePtr OmniverseScene::NodeById(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  NodePtr OmniverseScene::NodeByName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  NodePtr OmniverseScene::NodeByIndex(unsigned int _index) const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyNode(NodePtr _node, bool _recursive = false) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyNodeById(unsigned int _id) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyNodeByName(const std::string &_name) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyNodeByIndex(unsigned int _index) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyNodes() { throw std::runtime_error("not implemented"); }
  unsigned int OmniverseScene::LightCount() const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasLight(ConstLightPtr _light) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasLightId(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasLightName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  LightPtr OmniverseScene::LightById(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  LightPtr OmniverseScene::LightByName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  LightPtr OmniverseScene::LightByIndex(unsigned int _index) const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyLight(LightPtr _light, bool _recursive = false) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyLightById(unsigned int _id) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyLightByName(const std::string &_name) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyLightByIndex(unsigned int _index) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyLights() { throw std::runtime_error("not implemented"); }
  unsigned int OmniverseScene::SensorCount() const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasSensor(ConstSensorPtr _sensor) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasSensorId(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasSensorName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  SensorPtr OmniverseScene::SensorById(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  SensorPtr OmniverseScene::SensorByName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  SensorPtr OmniverseScene::SensorByIndex(unsigned int _index) const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroySensor(SensorPtr _sensor, bool _recursive = false) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroySensorById(unsigned int _id) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroySensorByName(const std::string &_name) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroySensorByIndex(unsigned int _index) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroySensors() { throw std::runtime_error("not implemented"); }
  unsigned int OmniverseScene::VisualCount() const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasVisual(ConstVisualPtr _node) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasVisualId(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::HasVisualName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::VisualById(unsigned int _id) const { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::VisualByName(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::VisualByIndex(unsigned int _index) const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyVisual(VisualPtr _node, bool _recursive = false) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyVisualById(unsigned int _id) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyVisualByName(const std::string &_name) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyVisualByIndex(unsigned int _index) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyVisuals() { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::MaterialRegistered(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  MaterialPtr OmniverseScene::Material(const std::string &_name) const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::RegisterMaterial(const std::string &_name, MaterialPtr _material) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::UnregisterMaterial(const std::string &_name) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::UnregisterMaterials() { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyMaterial(MaterialPtr _material) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::DestroyMaterials() { throw std::runtime_error("not implemented"); }
  DirectionalLightPtr OmniverseScene::CreateDirectionalLight() { throw std::runtime_error("not implemented"); }
  DirectionalLightPtr OmniverseScene::CreateDirectionalLight(unsigned int _id) { throw std::runtime_error("not implemented"); }
  DirectionalLightPtr OmniverseScene::CreateDirectionalLight(const std::string &_name) { throw std::runtime_error("not implemented"); }
  DirectionalLightPtr OmniverseScene::CreateDirectionalLight(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  PointLightPtr OmniverseScene::CreatePointLight() { throw std::runtime_error("not implemented"); }
  PointLightPtr OmniverseScene::CreatePointLight(unsigned int _id) { throw std::runtime_error("not implemented"); }
  PointLightPtr OmniverseScene::CreatePointLight(const std::string &_name) { throw std::runtime_error("not implemented"); }
  PointLightPtr OmniverseScene::CreatePointLight(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  SpotLightPtr OmniverseScene::CreateSpotLight() { throw std::runtime_error("not implemented"); }
  SpotLightPtr OmniverseScene::CreateSpotLight(unsigned int _id) { throw std::runtime_error("not implemented"); }
  SpotLightPtr OmniverseScene::CreateSpotLight(const std::string &_name) { throw std::runtime_error("not implemented"); }
  SpotLightPtr OmniverseScene::CreateSpotLight(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  CameraPtr OmniverseScene::CreateCamera() { throw std::runtime_error("not implemented"); }
  CameraPtr OmniverseScene::CreateCamera(unsigned int _id) { throw std::runtime_error("not implemented"); }
  CameraPtr OmniverseScene::CreateCamera(const std::string &_name) { throw std::runtime_error("not implemented"); }
  CameraPtr OmniverseScene::CreateCamera(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  DepthCameraPtr OmniverseScene::CreateDepthCamera() { throw std::runtime_error("not implemented"); }
  DepthCameraPtr OmniverseScene::CreateDepthCamera(unsigned int _id) { throw std::runtime_error("not implemented"); }
  DepthCameraPtr OmniverseScene::CreateDepthCamera(const std::string &_name) { throw std::runtime_error("not implemented"); }
  DepthCameraPtr OmniverseScene::CreateDepthCamera(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  ThermalCameraPtr OmniverseScene::CreateThermalCamera() { throw std::runtime_error("not implemented"); }
  ThermalCameraPtr OmniverseScene::CreateThermalCamera(unsigned int _id) { throw std::runtime_error("not implemented"); }
  ThermalCameraPtr OmniverseScene::CreateThermalCamera(const std::string &_name) { throw std::runtime_error("not implemented"); }
  ThermalCameraPtr OmniverseScene::CreateThermalCamera(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  BoundingBoxCameraPtr OmniverseScene::CreateBoundingBoxCamera() { throw std::runtime_error("not implemented"); }
  BoundingBoxCameraPtr OmniverseScene::CreateBoundingBoxCamera(unsigned int _id) { throw std::runtime_error("not implemented"); }
  BoundingBoxCameraPtr OmniverseScene::CreateBoundingBoxCamera(const std::string &_name) { throw std::runtime_error("not implemented"); }
  BoundingBoxCameraPtr OmniverseScene::CreateBoundingBoxCamera(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  SegmentationCameraPtr OmniverseScene::CreateSegmentationCamera() { throw std::runtime_error("not implemented"); }
  SegmentationCameraPtr OmniverseScene::CreateSegmentationCamera(unsigned int _id) { throw std::runtime_error("not implemented"); }
  SegmentationCameraPtr OmniverseScene::CreateSegmentationCamera(const std::string &_name) { throw std::runtime_error("not implemented"); }
  SegmentationCameraPtr OmniverseScene::CreateSegmentationCamera(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  GpuRaysPtr OmniverseScene::CreateGpuRays() { throw std::runtime_error("not implemented"); }
  GpuRaysPtr OmniverseScene::CreateGpuRays(unsigned int _id) { throw std::runtime_error("not implemented"); }
  GpuRaysPtr OmniverseScene::CreateGpuRays(const std::string &_name) { throw std::runtime_error("not implemented"); }
  GpuRaysPtr OmniverseScene::CreateGpuRays(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::CreateVisual() { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::CreateVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::CreateVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  VisualPtr OmniverseScene::CreateVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  ArrowVisualPtr OmniverseScene::CreateArrowVisual() { throw std::runtime_error("not implemented"); }
  ArrowVisualPtr OmniverseScene::CreateArrowVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  ArrowVisualPtr OmniverseScene::CreateArrowVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  ArrowVisualPtr OmniverseScene::CreateArrowVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  AxisVisualPtr OmniverseScene::CreateAxisVisual() { throw std::runtime_error("not implemented"); }
  AxisVisualPtr OmniverseScene::CreateAxisVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  AxisVisualPtr OmniverseScene::CreateAxisVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  AxisVisualPtr OmniverseScene::CreateAxisVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  GizmoVisualPtr OmniverseScene::CreateGizmoVisual() { throw std::runtime_error("not implemented"); }
  GizmoVisualPtr OmniverseScene::CreateGizmoVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  GizmoVisualPtr OmniverseScene::CreateGizmoVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  GizmoVisualPtr OmniverseScene::CreateGizmoVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  COMVisualPtr OmniverseScene::CreateCOMVisual() { throw std::runtime_error("not implemented"); }
  COMVisualPtr OmniverseScene::CreateCOMVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  COMVisualPtr OmniverseScene::CreateCOMVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  COMVisualPtr OmniverseScene::CreateCOMVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  InertiaVisualPtr OmniverseScene::CreateInertiaVisual() { throw std::runtime_error("not implemented"); }
  InertiaVisualPtr OmniverseScene::CreateInertiaVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  InertiaVisualPtr OmniverseScene::CreateInertiaVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  InertiaVisualPtr OmniverseScene::CreateInertiaVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  JointVisualPtr OmniverseScene::CreateJointVisual() { throw std::runtime_error("not implemented"); }
  JointVisualPtr OmniverseScene::CreateJointVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  JointVisualPtr OmniverseScene::CreateJointVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  JointVisualPtr OmniverseScene::CreateJointVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  LightVisualPtr OmniverseScene::CreateLightVisual() { throw std::runtime_error("not implemented"); }
  LightVisualPtr OmniverseScene::CreateLightVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  LightVisualPtr OmniverseScene::CreateLightVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  LightVisualPtr OmniverseScene::CreateLightVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  GeometryPtr OmniverseScene::CreateBox() { throw std::runtime_error("not implemented"); }
  CapsulePtr OmniverseScene::CreateCapsule() { throw std::runtime_error("not implemented"); }
  GeometryPtr OmniverseScene::CreateCone() { throw std::runtime_error("not implemented"); }
  GeometryPtr OmniverseScene::CreateCylinder() { throw std::runtime_error("not implemented"); }
  GeometryPtr OmniverseScene::CreatePlane() { throw std::runtime_error("not implemented"); }
  GeometryPtr OmniverseScene::CreateSphere() { throw std::runtime_error("not implemented"); }
  MeshPtr OmniverseScene::CreateMesh(const std::string &_meshName) { throw std::runtime_error("not implemented"); }
  MeshPtr OmniverseScene::CreateMesh(const common::Mesh *_mesh) { throw std::runtime_error("not implemented"); }
  MeshPtr OmniverseScene::CreateMesh(const MeshDescriptor &_desc) { throw std::runtime_error("not implemented"); }
  GridPtr OmniverseScene::CreateGrid() { throw std::runtime_error("not implemented"); }
  WireBoxPtr OmniverseScene::CreateWireBox() { throw std::runtime_error("not implemented"); }
  MarkerPtr OmniverseScene::CreateMarker() { throw std::runtime_error("not implemented"); }
  LidarVisualPtr OmniverseScene::CreateLidarVisual() { throw std::runtime_error("not implemented"); }
  LidarVisualPtr OmniverseScene::CreateLidarVisual(unsigned int _id) { throw std::runtime_error("not implemented"); }
  LidarVisualPtr OmniverseScene::CreateLidarVisual(const std::string &_name) { throw std::runtime_error("not implemented"); }
  LidarVisualPtr OmniverseScene::CreateLidarVisual(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  HeightmapPtr OmniverseScene::CreateHeightmap(const HeightmapDescriptor &_desc) { throw std::runtime_error("not implemented"); }
  TextPtr OmniverseScene::CreateText() { throw std::runtime_error("not implemented"); }
  MaterialPtr OmniverseScene::CreateMaterial(const std::string &_name = "") { throw std::runtime_error("not implemented"); }
  MaterialPtr OmniverseScene::CreateMaterial(const common::Material &_material) { throw std::runtime_error("not implemented"); }
  RenderTexturePtr OmniverseScene::CreateRenderTexture() { throw std::runtime_error("not implemented"); }
  RenderWindowPtr OmniverseScene::CreateRenderWindow() { throw std::runtime_error("not implemented"); }
  RayQueryPtr OmniverseScene::CreateRayQuery() { throw std::runtime_error("not implemented"); }
  ParticleEmitterPtr OmniverseScene::CreateParticleEmitter() { throw std::runtime_error("not implemented"); }
  ParticleEmitterPtr OmniverseScene::CreateParticleEmitter(unsigned int _id) { throw std::runtime_error("not implemented"); }
  ParticleEmitterPtr OmniverseScene::CreateParticleEmitter(const std::string &_name) { throw std::runtime_error("not implemented"); }
  ParticleEmitterPtr OmniverseScene::CreateParticleEmitter(unsigned int _id, const std::string &_name) { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetSkyEnabled(bool _enabled) { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::SkyEnabled() const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::PreRender() { throw std::runtime_error("not implemented"); }
  void OmniverseScene::PostRender() { throw std::runtime_error("not implemented"); }
  void OmniverseScene::SetCameraPassCountPerGpuFlush(uint8_t _numPass) { throw std::runtime_error("not implemented"); }
  uint8_t OmniverseScene::CameraPassCountPerGpuFlush() const { throw std::runtime_error("not implemented"); }
  bool OmniverseScene::LegacyAutoGpuFlush() const { throw std::runtime_error("not implemented"); }
  void OmniverseScene::Clear() { throw std::runtime_error("not implemented"); }
  void OmniverseScene::Destroy() { throw std::runtime_error("not implemented"); }

} // namespace ignition::rendering::omni