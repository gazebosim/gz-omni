/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not use this file except in compliance with the License.
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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSESCENE_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSESCENE_HH

#include <pxr/usd/usd/stage.h>
#include <ignition/rendering.hh>

namespace ignition::rendering::omni
{
  class OmniverseScene : public Scene
  {
  public:
    void Load() override;
    void Init() override;
    void Fini() override;
    bool IsInitialized() const override;
    unsigned int Id() const override;
    std::string Name() const override;
    RenderEngine *Engine() const override;
    common::Time IGN_DEPRECATED(4) SimTime() const override;
    std::chrono::steady_clock::duration Time() const override;
    void IGN_DEPRECATED(4) SetSimTime(const common::Time &_time) override;
    void SetTime(const std::chrono::steady_clock::duration &_time) override;
    VisualPtr RootVisual() const override;
    VisualPtr VisualAt(const CameraPtr &_camera, const math::Vector2i &_mousePos) override;
    math::Color AmbientLight() const override;
    void SetAmbientLight(double _r, double _g, double _b, double _a = 1.0) override;
    void SetAmbientLight(const math::Color &_color) override;
    math::Color BackgroundColor() const override;
    void SetBackgroundColor(double _r, double _g, double _b, double _a = 1.0) override;
    void SetBackgroundColor(const math::Color &_color) override;
    bool IsGradientBackgroundColor() const override;
    std::array<math::Color, 4> GradientBackgroundColor() const override;
    void SetGradientBackgroundColor(const std::array<math::Color, 4> &_colors) override;
    void RemoveGradientBackgroundColor() override;
    MaterialPtr BackgroundMaterial() const override;
    void SetBackgroundMaterial(MaterialPtr _material) override;
    unsigned int NodeCount() const override;
    bool HasNode(ConstNodePtr _node) const override;
    bool HasNodeId(unsigned int _id) const override;
    bool HasNodeName(const std::string &_name) const override;
    NodePtr NodeById(unsigned int _id) const override;
    NodePtr NodeByName(const std::string &_name) const override;
    NodePtr NodeByIndex(unsigned int _index) const override;
    void DestroyNode(NodePtr _node, bool _recursive = false) override;
    void DestroyNodeById(unsigned int _id) override;
    void DestroyNodeByName(const std::string &_name) override;
    void DestroyNodeByIndex(unsigned int _index) override;
    void DestroyNodes() override;
    unsigned int LightCount() const override;
    bool HasLight(ConstLightPtr _light) const override;
    bool HasLightId(unsigned int _id) const override;
    bool HasLightName(const std::string &_name) const override;
    LightPtr LightById(unsigned int _id) const override;
    LightPtr LightByName(const std::string &_name) const override;
    LightPtr LightByIndex(unsigned int _index) const override;
    void DestroyLight(LightPtr _light, bool _recursive = false) override;
    void DestroyLightById(unsigned int _id) override;
    void DestroyLightByName(const std::string &_name) override;
    void DestroyLightByIndex(unsigned int _index) override;
    void DestroyLights() override;
    unsigned int SensorCount() const override;
    bool HasSensor(ConstSensorPtr _sensor) const override;
    bool HasSensorId(unsigned int _id) const override;
    bool HasSensorName(const std::string &_name) const override;
    SensorPtr SensorById(unsigned int _id) const override;
    SensorPtr SensorByName(const std::string &_name) const override;
    SensorPtr SensorByIndex(unsigned int _index) const override;
    void DestroySensor(SensorPtr _sensor, bool _recursive = false) override;
    void DestroySensorById(unsigned int _id) override;
    void DestroySensorByName(const std::string &_name) override;
    void DestroySensorByIndex(unsigned int _index) override;
    void DestroySensors() override;
    unsigned int VisualCount() const override;
    bool HasVisual(ConstVisualPtr _node) const override;
    bool HasVisualId(unsigned int _id) const override;
    bool HasVisualName(const std::string &_name) const override;
    VisualPtr VisualById(unsigned int _id) const override;
    VisualPtr VisualByName(const std::string &_name) const override;
    VisualPtr VisualByIndex(unsigned int _index) const override;
    void DestroyVisual(VisualPtr _node, bool _recursive = false) override;
    void DestroyVisualById(unsigned int _id) override;
    void DestroyVisualByName(const std::string &_name) override;
    void DestroyVisualByIndex(unsigned int _index) override;
    void DestroyVisuals() override;
    bool MaterialRegistered(const std::string &_name) const override;
    MaterialPtr Material(const std::string &_name) const override;
    void RegisterMaterial(const std::string &_name, MaterialPtr _material) override;
    void UnregisterMaterial(const std::string &_name) override;
    void UnregisterMaterials() override;
    void DestroyMaterial(MaterialPtr _material) override;
    void DestroyMaterials() override;
    DirectionalLightPtr CreateDirectionalLight() override;
    DirectionalLightPtr CreateDirectionalLight(unsigned int _id) override;
    DirectionalLightPtr CreateDirectionalLight(const std::string &_name) override;
    DirectionalLightPtr CreateDirectionalLight(unsigned int _id, const std::string &_name) override;
    PointLightPtr CreatePointLight() override;
    PointLightPtr CreatePointLight(unsigned int _id) override;
    PointLightPtr CreatePointLight(const std::string &_name) override;
    PointLightPtr CreatePointLight(unsigned int _id, const std::string &_name) override;
    SpotLightPtr CreateSpotLight() override;
    SpotLightPtr CreateSpotLight(unsigned int _id) override;
    SpotLightPtr CreateSpotLight(const std::string &_name) override;
    SpotLightPtr CreateSpotLight(unsigned int _id, const std::string &_name) override;
    CameraPtr CreateCamera() override;
    CameraPtr CreateCamera(unsigned int _id) override;
    CameraPtr CreateCamera(const std::string &_name) override;
    CameraPtr CreateCamera(unsigned int _id, const std::string &_name) override;
    DepthCameraPtr CreateDepthCamera() override;
    DepthCameraPtr CreateDepthCamera(unsigned int _id) override;
    DepthCameraPtr CreateDepthCamera(const std::string &_name) override;
    DepthCameraPtr CreateDepthCamera(unsigned int _id, const std::string &_name) override;
    ThermalCameraPtr CreateThermalCamera() override;
    ThermalCameraPtr CreateThermalCamera(unsigned int _id) override;
    ThermalCameraPtr CreateThermalCamera(const std::string &_name) override;
    ThermalCameraPtr CreateThermalCamera(unsigned int _id, const std::string &_name) override;
    BoundingBoxCameraPtr CreateBoundingBoxCamera() override;
    BoundingBoxCameraPtr CreateBoundingBoxCamera(unsigned int _id) override;
    BoundingBoxCameraPtr CreateBoundingBoxCamera(const std::string &_name) override;
    BoundingBoxCameraPtr CreateBoundingBoxCamera(unsigned int _id, const std::string &_name) override;
    SegmentationCameraPtr CreateSegmentationCamera() override;
    SegmentationCameraPtr CreateSegmentationCamera(unsigned int _id) override;
    SegmentationCameraPtr CreateSegmentationCamera(const std::string &_name) override;
    SegmentationCameraPtr CreateSegmentationCamera(unsigned int _id, const std::string &_name) override;
    GpuRaysPtr CreateGpuRays() override;
    GpuRaysPtr CreateGpuRays(unsigned int _id) override;
    GpuRaysPtr CreateGpuRays(const std::string &_name) override;
    GpuRaysPtr CreateGpuRays(unsigned int _id, const std::string &_name) override;
    VisualPtr CreateVisual() override;
    VisualPtr CreateVisual(unsigned int _id) override;
    VisualPtr CreateVisual(const std::string &_name) override;
    VisualPtr CreateVisual(unsigned int _id, const std::string &_name) override;
    ArrowVisualPtr CreateArrowVisual() override;
    ArrowVisualPtr CreateArrowVisual(unsigned int _id) override;
    ArrowVisualPtr CreateArrowVisual(const std::string &_name) override;
    ArrowVisualPtr CreateArrowVisual(unsigned int _id, const std::string &_name) override;
    AxisVisualPtr CreateAxisVisual() override;
    AxisVisualPtr CreateAxisVisual(unsigned int _id) override;
    AxisVisualPtr CreateAxisVisual(const std::string &_name) override;
    AxisVisualPtr CreateAxisVisual(unsigned int _id, const std::string &_name) override;
    GizmoVisualPtr CreateGizmoVisual() override;
    GizmoVisualPtr CreateGizmoVisual(unsigned int _id) override;
    GizmoVisualPtr CreateGizmoVisual(const std::string &_name) override;
    GizmoVisualPtr CreateGizmoVisual(unsigned int _id, const std::string &_name) override;
    COMVisualPtr CreateCOMVisual() override;
    COMVisualPtr CreateCOMVisual(unsigned int _id) override;
    COMVisualPtr CreateCOMVisual(const std::string &_name) override;
    COMVisualPtr CreateCOMVisual(unsigned int _id, const std::string &_name) override;
    InertiaVisualPtr CreateInertiaVisual() override;
    InertiaVisualPtr CreateInertiaVisual(unsigned int _id) override;
    InertiaVisualPtr CreateInertiaVisual(const std::string &_name) override;
    InertiaVisualPtr CreateInertiaVisual(unsigned int _id, const std::string &_name) override;
    JointVisualPtr CreateJointVisual() override;
    JointVisualPtr CreateJointVisual(unsigned int _id) override;
    JointVisualPtr CreateJointVisual(const std::string &_name) override;
    JointVisualPtr CreateJointVisual(unsigned int _id, const std::string &_name) override;
    LightVisualPtr CreateLightVisual() override;
    LightVisualPtr CreateLightVisual(unsigned int _id) override;
    LightVisualPtr CreateLightVisual(const std::string &_name) override;
    LightVisualPtr CreateLightVisual(unsigned int _id, const std::string &_name) override;
    GeometryPtr CreateBox() override;
    CapsulePtr CreateCapsule() override;
    GeometryPtr CreateCone() override;
    GeometryPtr CreateCylinder() override;
    GeometryPtr CreatePlane() override;
    GeometryPtr CreateSphere() override;
    MeshPtr CreateMesh(const std::string &_meshName) override;
    MeshPtr CreateMesh(const common::Mesh *_mesh) override;
    MeshPtr CreateMesh(const MeshDescriptor &_desc) override;
    GridPtr CreateGrid() override;
    WireBoxPtr CreateWireBox() override;
    MarkerPtr CreateMarker() override;
    LidarVisualPtr CreateLidarVisual() override;
    LidarVisualPtr CreateLidarVisual(unsigned int _id) override;
    LidarVisualPtr CreateLidarVisual(const std::string &_name) override;
    LidarVisualPtr CreateLidarVisual(unsigned int _id, const std::string &_name) override;
    HeightmapPtr CreateHeightmap(const HeightmapDescriptor &_desc) override;
    TextPtr CreateText() override;
    MaterialPtr CreateMaterial(const std::string &_name = "") override;
    MaterialPtr CreateMaterial(const common::Material &_material) override;
    RenderTexturePtr CreateRenderTexture() override;
    RenderWindowPtr CreateRenderWindow() override;
    RayQueryPtr CreateRayQuery() override;
    ParticleEmitterPtr CreateParticleEmitter() override;
    ParticleEmitterPtr CreateParticleEmitter(unsigned int _id) override;
    ParticleEmitterPtr CreateParticleEmitter(const std::string &_name) override;
    ParticleEmitterPtr CreateParticleEmitter(unsigned int _id, const std::string &_name) override;
    void SetSkyEnabled(bool _enabled) override;
    bool SkyEnabled() const override;
    void PreRender() override;
    void PostRender() override;
    void SetCameraPassCountPerGpuFlush(uint8_t _numPass) override;
    uint8_t CameraPassCountPerGpuFlush() const override;
    bool LegacyAutoGpuFlush() const override;
    void Clear() override;
    void Destroy() override;
  };
} // namespace ignition::rendering::omni

#endif
