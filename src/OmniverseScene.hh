/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
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

#include <ignition/rendering/base/BaseScene.hh>

namespace ignition::rendering::omni
{
  class OmniverseScene : public BaseScene
  {
  public:
    OmniverseScene(RenderEngine *engine, pxr::UsdStageRefPtr stage, unsigned int id, std::string name);

    ~OmniverseScene() override;

    inline RenderEngine *Engine() const override { return this->_engine; }

    VisualPtr RootVisual() const override;

    math::Color AmbientLight() const override;

    void SetAmbientLight(const math::Color &_color) override;

    COMVisualPtr CreateCOMVisualImpl(unsigned int _id, const std::string &_name) override;

    InertiaVisualPtr CreateInertiaVisualImpl(unsigned int _id, const std::string &_name) override;

    JointVisualPtr CreateJointVisualImpl(unsigned int _id, const std::string &_name) override;

    LightVisualPtr CreateLightVisualImpl(unsigned int _id, const std::string &_name) override;

    DirectionalLightPtr CreateDirectionalLightImpl(unsigned int _id, const std::string &_name) override;

    PointLightPtr CreatePointLightImpl(unsigned int _id, const std::string &_name) override;

    SpotLightPtr CreateSpotLightImpl(unsigned int _id, const std::string &_name) override;

    CameraPtr CreateCameraImpl(unsigned int _id, const std::string &_name) override;

    DepthCameraPtr CreateDepthCameraImpl(unsigned int _id, const std::string &_name) override;

    VisualPtr CreateVisualImpl(unsigned int _id, const std::string &_name) override;

    ArrowVisualPtr CreateArrowVisualImpl(unsigned int _id, const std::string &_name) override;

    AxisVisualPtr CreateAxisVisualImpl(unsigned int _id, const std::string &_name) override;

    GeometryPtr CreateBoxImpl(unsigned int _id, const std::string &_name) override;

    GeometryPtr CreateConeImpl(unsigned int _id, const std::string &_name) override;

    GeometryPtr CreateCylinderImpl(unsigned int _id, const std::string &_name) override;

    GeometryPtr CreatePlaneImpl(unsigned int _id, const std::string &_name) override;

    GeometryPtr CreateSphereImpl(unsigned int _id, const std::string &_name) override;

    MeshPtr CreateMeshImpl(unsigned int _id,
                           const std::string &_name, const MeshDescriptor &_desc) override;

    CapsulePtr CreateCapsuleImpl(unsigned int _id, const std::string &_name) override;

    GridPtr CreateGridImpl(unsigned int _id, const std::string &_name) override;

    MarkerPtr CreateMarkerImpl(unsigned int _id, const std::string &_name) override;

    LidarVisualPtr CreateLidarVisualImpl(unsigned int _id, const std::string &_name) override;

    HeightmapPtr CreateHeightmapImpl(unsigned int _id,
                                     const std::string &_name, const HeightmapDescriptor &_desc) override;

    WireBoxPtr CreateWireBoxImpl(unsigned int _id, const std::string &_name) override;

    MaterialPtr CreateMaterialImpl(unsigned int _id, const std::string &_name) override;

    RenderTexturePtr CreateRenderTextureImpl(unsigned int _id, const std::string &_name) override;

    RenderWindowPtr CreateRenderWindowImpl(unsigned int _id, const std::string &_name) override;

    RayQueryPtr CreateRayQueryImpl(unsigned int _id, const std::string &_name) override;

    LightStorePtr Lights() const override;

    SensorStorePtr Sensors() const override;

    VisualStorePtr Visuals() const override;

    MaterialMapPtr Materials() const override;

    bool LoadImpl() override;

    bool InitImpl() override;

  private:
    pxr::UsdStageRefPtr _stage;
    pxr::SdfLayerHandle _layer;
    RenderEngine *_engine;
  };
}

#endif
