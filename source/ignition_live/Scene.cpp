/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include "Scene.hpp"

#include "Material.hpp"
#include "Mesh.hpp"

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/math/Quaternion.hh>

#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdLux/diskLight.h>
#include <pxr/usd/usdLux/distantLight.h>
#include <pxr/usd/usdLux/sphereLight.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

namespace ignition
{
namespace omniverse
{
class Scene::Implementation
{
 public:
  std::string worldName;
  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> stage;
  std::shared_ptr<ignition::transport::Node> node;
  std::string stageDirUrl;
  std::unordered_map<uint32_t, pxr::UsdPrim> entities;

  bool UpdateLights(const ignition::msgs::Light &_light,
                    const std::string &_usdLightPath);
  bool UpdateScene(const ignition::msgs::Scene &_scene);
  bool UpdateVisual(const ignition::msgs::Visual &_visual,
                    const std::string &_usdPath);
  bool UpdateLink(const ignition::msgs::Link &_link,
                  const std::string &_usdModelPath);
  bool UpdateJoint(const ignition::msgs::Joint &_joint);
  bool UpdateModel(const ignition::msgs::Model &_model);
  void SetPose(const pxr::UsdGeomXformCommonAPI &_prim,
               const ignition::msgs::Pose &_pose);
  void ResetPose(const pxr::UsdGeomXformCommonAPI &_prim);
  void SetScale(const pxr::UsdGeomXformCommonAPI &_xform,
                const ignition::msgs::Vector3d &_scale);
  void ResetScale(const pxr::UsdGeomXformCommonAPI &_prim);
  void CallbackPoses(const ignition::msgs::Pose_V &_msg);
  void CallbackJoint(const ignition::msgs::Model &_msg);
  void CallbackScene(const ignition::msgs::Scene &_scene);
  void CallbackSceneDeletion(const ignition::msgs::UInt32_V &_msg);
};

//////////////////////////////////////////////////
Scene::Scene(const std::string &_worldName, const std::string &_stageUrl)
    : dataPtr(ignition::utils::MakeImpl<Implementation>())

{
  ignmsg << "Opened stage [" << _stageUrl << "]" << std::endl;
  this->dataPtr->worldName = _worldName;
  this->dataPtr->stage = std::make_shared<ThreadSafe<pxr::UsdStageRefPtr>>(
      pxr::UsdStage::Open(_stageUrl));
  this->dataPtr->node = std::make_shared<ignition::transport::Node>();
  this->dataPtr->stageDirUrl = ignition::common::parentPath(_stageUrl);
}

// //////////////////////////////////////////////////
std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> &Scene::Stage()
{
  return this->dataPtr->stage;
}

//////////////////////////////////////////////////
void Scene::Implementation::SetPose(const pxr::UsdGeomXformCommonAPI &_prim,
                                    const ignition::msgs::Pose &_pose)
{
  pxr::UsdGeomXformCommonAPI xformApi(_prim);
  const auto &pos = _pose.position();
  const auto &orient = _pose.orientation();
  ignition::math::Quaterniond quat(orient.w(), orient.x(), orient.y(),
                                   orient.z());
  xformApi.SetTranslate(pxr::GfVec3d(pos.x(), pos.y(), pos.z()));
  xformApi.SetRotate(pxr::GfVec3f(ignition::math::Angle(quat.Roll()).Degree(),
                                  ignition::math::Angle(quat.Pitch()).Degree(),
                                  ignition::math::Angle(quat.Yaw()).Degree()),
                     pxr::UsdGeomXformCommonAPI::RotationOrderXYZ);
}

//////////////////////////////////////////////////
void Scene::Implementation::ResetPose(const pxr::UsdGeomXformCommonAPI &_prim)
{
  pxr::UsdGeomXformCommonAPI xformApi(_prim);
  xformApi.SetTranslate(pxr::GfVec3d(0));
  xformApi.SetRotate(pxr::GfVec3f(0));
}

//////////////////////////////////////////////////
void Scene::Implementation::SetScale(const pxr::UsdGeomXformCommonAPI &_prim,
                                     const ignition::msgs::Vector3d &_scale)
{
  pxr::UsdGeomXformCommonAPI xformApi(_prim);
  xformApi.SetScale(pxr::GfVec3f(_scale.x(), _scale.y(), _scale.z()));
}

//////////////////////////////////////////////////
void Scene::Implementation::ResetScale(const pxr::UsdGeomXformCommonAPI &_prim)
{
  pxr::UsdGeomXformCommonAPI xformApi(_prim);
  xformApi.SetScale(pxr::GfVec3f(1));
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateVisual(const ignition::msgs::Visual &_visual,
                                         const std::string &_usdLinkPath)
{
  auto stage = this->stage->Lock();

  std::string usdVisualPath = _usdLinkPath + "/" + _visual.name();
  auto usdVisualXform =
      pxr::UsdGeomXform::Define(*stage, pxr::SdfPath(usdVisualPath));
  pxr::UsdGeomXformCommonAPI xformApi(usdVisualXform);
  if (_visual.has_scale())
  {
    this->SetScale(xformApi, _visual.scale());
  }
  else
  {
    this->ResetScale(xformApi);
  }
  if (_visual.has_pose())
  {
    this->SetPose(xformApi, _visual.pose());
  }
  else
  {
    this->ResetPose(xformApi);
  }
  this->entities[_visual.id()] = usdVisualXform.GetPrim();

  std::string usdGeomPath(usdVisualPath + "/geometry");
  const auto &geom = _visual.geometry();

  switch (geom.type())
  {
    case ignition::msgs::Geometry::BOX:
    {
      auto usdCube =
          pxr::UsdGeomCube::Define(*stage, pxr::SdfPath(usdGeomPath));
      usdCube.CreateSizeAttr().Set(1.0);
      pxr::GfVec3f endPoint(0.5);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCube.CreateExtentAttr().Set(extentBounds);
      pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
      cubeXformAPI.SetScale(pxr::GfVec3f(
          geom.box().size().x(), geom.box().size().y(), geom.box().size().z()));
      if (!SetMaterial(usdCube, _visual, *stage))
      {
        ignwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    // TODO: Support cone
    // case ignition::msgs::Geometry::CONE:
    case ignition::msgs::Geometry::CYLINDER:
    {
      auto usdCylinder =
          pxr::UsdGeomCylinder::Define(*stage, pxr::SdfPath(usdGeomPath));
      double radius = geom.cylinder().radius();
      double length = geom.cylinder().length();

      usdCylinder.CreateRadiusAttr().Set(radius);
      usdCylinder.CreateHeightAttr().Set(length);
      pxr::GfVec3f endPoint(radius);
      endPoint[2] = length * 0.5;
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCylinder.CreateExtentAttr().Set(extentBounds);
      if (!SetMaterial(usdCylinder, _visual, *stage))
      {
        ignwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case ignition::msgs::Geometry::PLANE:
    {
      auto usdCube =
          pxr::UsdGeomCube::Define(*stage, pxr::SdfPath(usdGeomPath));
      usdCube.CreateSizeAttr().Set(1.0);
      pxr::GfVec3f endPoint(0.5);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCube.CreateExtentAttr().Set(extentBounds);

      pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
      cubeXformAPI.SetScale(pxr::GfVec3f(geom.plane().size().x(),
                                         geom.plane().size().y(), 0.0025));
      if (!SetMaterial(usdCube, _visual, *stage))
      {
        ignwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case ignition::msgs::Geometry::ELLIPSOID:
    {
      auto usdEllipsoid =
          pxr::UsdGeomSphere::Define(*stage, pxr::SdfPath(usdGeomPath));
      const auto maxRadii =
          ignition::math::Vector3d(geom.ellipsoid().radii().x(),
                                   geom.ellipsoid().radii().y(),
                                   geom.ellipsoid().radii().z())
              .Max();
      usdEllipsoid.CreateRadiusAttr().Set(0.5);
      pxr::UsdGeomXformCommonAPI xform(usdEllipsoid);
      xform.SetScale(pxr::GfVec3f{
          static_cast<float>(geom.ellipsoid().radii().x() / maxRadii),
          static_cast<float>(geom.ellipsoid().radii().y() / maxRadii),
          static_cast<float>(geom.ellipsoid().radii().z() / maxRadii),
      });
      // extents is the bounds before any transformation
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(pxr::GfVec3f{static_cast<float>(-maxRadii)});
      extentBounds.push_back(pxr::GfVec3f{static_cast<float>(maxRadii)});
      usdEllipsoid.CreateExtentAttr().Set(extentBounds);
      if (!SetMaterial(usdEllipsoid, _visual, *stage))
      {
        ignwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case ignition::msgs::Geometry::SPHERE:
    {
      auto usdSphere =
          pxr::UsdGeomSphere::Define(*stage, pxr::SdfPath(usdGeomPath));
      double radius = geom.sphere().radius();
      usdSphere.CreateRadiusAttr().Set(radius);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(pxr::GfVec3f(-1.0 * radius));
      extentBounds.push_back(pxr::GfVec3f(radius));
      usdSphere.CreateExtentAttr().Set(extentBounds);
      if (!SetMaterial(usdSphere, _visual, *stage))
      {
        ignwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case ignition::msgs::Geometry::CAPSULE:
    {
      auto usdCapsule =
          pxr::UsdGeomCapsule::Define(*stage, pxr::SdfPath(usdGeomPath));
      double radius = geom.capsule().radius();
      double length = geom.capsule().length();
      usdCapsule.CreateRadiusAttr().Set(radius);
      usdCapsule.CreateHeightAttr().Set(length);
      pxr::GfVec3f endPoint(radius);
      endPoint[2] += 0.5 * length;
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCapsule.CreateExtentAttr().Set(extentBounds);
      if (!SetMaterial(usdCapsule, _visual, *stage))
      {
        ignwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case ignition::msgs::Geometry::MESH:
    {
      auto usdMesh = UpdateMesh(geom.mesh(), usdGeomPath, *stage);
      if (!usdMesh)
      {
        ignerr << "Failed to update visual [" << _visual.name() << "]"
               << std::endl;
        return false;
      }
      if (!SetMaterial(usdMesh, _visual, *stage))
      {
        ignerr << "Failed to update visual [" << _visual.name() << "]"
               << std::endl;
        return false;
      }
      break;
    }
    default:
      ignerr << "Failed to update geometry (unsuported geometry type '"
             << _visual.type() << "')" << std::endl;
      return false;
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateLink(const ignition::msgs::Link &_link,
                                       const std::string &_usdModelPath)
{
  auto stage = this->stage->Lock();
  std::string usdLinkPath = _usdModelPath + "/" + _link.name();
  auto xform = pxr::UsdGeomXform::Define(*stage, pxr::SdfPath(usdLinkPath));
  pxr::UsdGeomXformCommonAPI xformApi(xform);

  if (_link.has_pose())
  {
    this->SetPose(xformApi, _link.pose());
  }
  else
  {
    this->ResetPose(xformApi);
  }
  this->entities[_link.id()] = xform.GetPrim();

  for (const auto &visual : _link.visual())
  {
    if (!this->UpdateVisual(visual, usdLinkPath))
    {
      ignerr << "Failed to update link [" << _link.name() << "]" << std::endl;
      return false;
    }
  }

  for (const auto &light : _link.light())
  {
    if (!this->UpdateLights(light, usdLinkPath + "/" + light.name()))
    {
      ignerr << "Failed to add light [" << usdLinkPath + "/" + light.name()
             << "]" << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateJoint(const ignition::msgs::Joint &_joint)
{
  // TODO: this is not tested
  auto stage = this->stage->Lock();
  auto jointUSD =
      stage->GetPrimAtPath(pxr::SdfPath("/" + worldName + "/" + _joint.name()));
  // TODO(ahcorde): This code is duplicated in the sdformat converter.
  if (!jointUSD)
  {
    switch (_joint.type())
    {
      case ignition::msgs::Joint::FIXED:
      {
        pxr::TfToken usdPrimTypeName("PhysicsFixedJoint");
        auto jointFixedUSD = stage->DefinePrim(
          pxr::SdfPath("/" + this->worldName + "/" + _joint.name()),
          usdPrimTypeName);

        auto body0 = jointFixedUSD.CreateRelationship(
          pxr::TfToken("physics:body0"), false);
        body0.AddTarget(pxr::SdfPath(
          "/" + this->worldName + "/" + _joint.parent()));
        auto body1 = jointFixedUSD.CreateRelationship(
          pxr::TfToken("physics:body1"), false);
        body1.AddTarget(pxr::SdfPath(
          "/" + this->worldName + "/" + _joint.child()));

        jointFixedUSD.CreateAttribute(pxr::TfToken("physics:localPos1"),
                pxr::SdfValueTypeNames->Point3fArray, false).Set(
                  pxr::GfVec3f(0, 0, 0));

        jointFixedUSD.CreateAttribute(pxr::TfToken("physics:localPos0"),
                pxr::SdfValueTypeNames->Point3fArray, false).Set(
                  pxr::GfVec3f(_joint.pose().position().x(),
                               _joint.pose().position().y(),
                               _joint.pose().position().z()));
        return true;
      }
      case ignition::msgs::Joint::REVOLUTE:
      {
        igndbg << "Creating a revolute joint" << '\n';

        pxr::TfToken usdPrimTypeName("PhysicsRevoluteJoint");
        auto revoluteJointUSD = stage->DefinePrim(
          pxr::SdfPath("/" + this->worldName + "/" + _joint.name()),
          usdPrimTypeName);

        igndbg << "\tParent "
               << "/" + this->worldName + "/" + _joint.parent() << '\n';
        igndbg << "\tchild "
               << "/" + this->worldName + "/" + _joint.child() << '\n';

        pxr::TfTokenVector identifiersBody0 =
            {pxr::TfToken("physics"), pxr::TfToken("body0")};

        if (pxr::UsdRelationship body0 = revoluteJointUSD.CreateRelationship(
          pxr::TfToken(pxr::SdfPath::JoinIdentifier(identifiersBody0)), false))
        {
          body0.AddTarget(
            pxr::SdfPath("/" + this->worldName + "/panda/" + _joint.parent()),
            pxr::UsdListPositionFrontOfAppendList);
        }
        else
        {
          igndbg << "Not able to create UsdRelationship for body1" << '\n';
        }

        pxr::TfTokenVector identifiersBody1 =
            {pxr::TfToken("physics"), pxr::TfToken("body1")};

        if (pxr::UsdRelationship body1 = revoluteJointUSD.CreateRelationship(
          pxr::TfToken(pxr::SdfPath::JoinIdentifier(identifiersBody1)), false))
        {
          body1.AddTarget(
            pxr::SdfPath("/" + this->worldName + "/panda/" + _joint.child()),
            pxr::UsdListPositionFrontOfAppendList);
        }
        else
        {
          igndbg << "Not able to create UsdRelationship for body1" << '\n';
        }

        ignition::math::Vector3i axis(
          _joint.axis1().xyz().x(),
          _joint.axis1().xyz().y(),
          _joint.axis1().xyz().z());

        if (axis == ignition::math::Vector3i(1, 0, 0))
        {
          revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
            pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("X"));
        }
        else if (axis == ignition::math::Vector3i(0, 1, 0))
        {
          revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
            pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("Y"));
        }
        else if (axis == ignition::math::Vector3i(0, 0, 1))
        {
          revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
            pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("Z"));
        }

        revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:localPos1"),
                pxr::SdfValueTypeNames->Point3f, false).Set(
                  pxr::GfVec3f(0, 0, 0));

        revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:localPos0"),
          pxr::SdfValueTypeNames->Point3f, false).Set(
            pxr::GfVec3f(
              _joint.pose().position().x(),
              _joint.pose().position().y(),
              _joint.pose().position().z()));

        revoluteJointUSD.CreateAttribute(
          pxr::TfToken("drive:angular:physics:damping"),
          pxr::SdfValueTypeNames->Float, false).Set(100000.0f);
        revoluteJointUSD.CreateAttribute(
          pxr::TfToken("drive:angular:physics:stiffness"),
          pxr::SdfValueTypeNames->Float, false).Set(1000000.0f);

        revoluteJointUSD.CreateAttribute(
          pxr::TfToken("drive:angular:physics:targetPosition"),
          pxr::SdfValueTypeNames->Float, false).Set(0.0f);

        revoluteJointUSD.CreateAttribute(
          pxr::TfToken("physics:lowerLimit"),
          pxr::SdfValueTypeNames->Float, false).Set(
            static_cast<float>(_joint.axis1().limit_lower() * 180 / 3.1416));

        revoluteJointUSD.CreateAttribute(
          pxr::TfToken("physics:upperLimit"),
          pxr::SdfValueTypeNames->Float, false).Set(
            static_cast<float>(_joint.axis1().limit_upper() * 180 / 3.1416));

        pxr::TfToken appliedSchemaNamePhysicsArticulationRootAPI(
          "PhysicsArticulationRootAPI");
        pxr::TfToken appliedSchemaNamePhysxArticulationAPI(
          "PhysxArticulationAPI");
        pxr::SdfPrimSpecHandle primSpecPanda = pxr::SdfCreatePrimInLayer(
          stage->GetEditTarget().GetLayer(),
          pxr::SdfPath("/" + this->worldName + "/panda"));
        pxr::SdfTokenListOp listOpPanda;
        // Use ReplaceOperations to append in place.
        if (!listOpPanda.ReplaceOperations(
          pxr::SdfListOpTypeExplicit,
          0,
          0,
          {appliedSchemaNamePhysicsArticulationRootAPI,
           appliedSchemaNamePhysxArticulationAPI})) {
          ignerr << "Not able to setup the schema PhysxArticulationAPI "
                 << "and PhysicsArticulationRootAPI\n";
        }
        primSpecPanda->SetInfo(
          pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOpPanda));

        pxr::TfToken appliedSchemaName("PhysicsDriveAPI:angular");
        pxr::SdfPrimSpecHandle primSpec = pxr::SdfCreatePrimInLayer(
          stage->GetEditTarget().GetLayer(),
          pxr::SdfPath("/" + this->worldName + "/" + _joint.name()));
        pxr::SdfTokenListOp listOp;

        // Use ReplaceOperations to append in place.
        if (!listOp.ReplaceOperations(pxr::SdfListOpTypeExplicit,
                0, 0, {appliedSchemaName})) {
          ignerr << "Not able to setup the schema PhysicsDriveAPI\n";
        }

        primSpec->SetInfo(
          pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOp));
        break;
      }
      default:
        return false;
    }
  }
  // auto driveJoint = pxr::UsdPhysicsDriveAPI(jointUSD);
  auto attrTargetPos = jointUSD.GetAttribute(
      pxr::TfToken("drive:angular:physics:targetPosition"));
  if (attrTargetPos)
  {
    float pos;
    attrTargetPos.Get(&pos);
    attrTargetPos.Set(pxr::VtValue(
        ignition::math::Angle(_joint.axis1().position()).Degree()));
  }
  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateModel(const ignition::msgs::Model &_model)
{
  auto stage = this->stage->Lock();

  std::string modelName = _model.name();
  std::replace(modelName.begin(), modelName.end(), ' ', '_');

  std::string usdModelPath = "/" + worldName + "/" + modelName;
  auto xform = pxr::UsdGeomXform::Define(*stage, pxr::SdfPath(usdModelPath));
  pxr::UsdGeomXformCommonAPI xformApi(xform);
  if (_model.has_scale())
  {
    this->SetScale(xformApi, _model.scale());
  }
  else
  {
    this->ResetScale(xformApi);
  }
  if (_model.has_pose())
  {
    this->SetPose(xformApi, _model.pose());
  }
  else
  {
    this->ResetPose(xformApi);
  }
  this->entities[_model.id()] = xform.GetPrim();

  for (const auto &link : _model.link())
  {
    if (!this->UpdateLink(link, usdModelPath))
    {
      ignerr << "Failed to update model [" << modelName << "]" << std::endl;
      return false;
    }
  }

  for (const auto &joint : _model.joint())
  {
    if (!this->UpdateJoint(joint))
    {
      ignerr << "Failed to update model [" << modelName << "]" << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateScene(const ignition::msgs::Scene &_scene)
{
  for (const auto &model : _scene.model())
  {
    if (!this->UpdateModel(model))
    {
      ignerr << "Failed to add model [" << model.name() << "]" << std::endl;
      return false;
    }
    igndbg << "added model [" << model.name() << "]" << std::endl;
  }

  for (const auto &light : _scene.light())
  {
    if (!this->UpdateLights(light, "/" + worldName + "/" + light.name()))
    {
      ignerr << "Failed to add light [" << light.name() << "]" << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateLights(const ignition::msgs::Light &_light,
                                       const std::string &_usdLightPath)
{
  // TODO: We can probably re-use code from sdformat

  auto stage = this->stage->Lock();

  const pxr::SdfPath sdfLightPath(_usdLightPath);
  switch (_light.type())
  {
    case ignition::msgs::Light::POINT:
    {
      auto pointLight = pxr::UsdLuxSphereLight::Define(*stage, sdfLightPath);
      pointLight.CreateTreatAsPointAttr().Set(true);
      this->entities[_light.id()] = pointLight.GetPrim();
      pointLight.CreateRadiusAttr(pxr::VtValue(0.1f));
      pointLight.CreateColorAttr(pxr::VtValue(pxr::GfVec3f(
          _light.diffuse().r(), _light.diffuse().g(), _light.diffuse().b())));
      break;
    }
    case ignition::msgs::Light::SPOT:
    {
      auto diskLight = pxr::UsdLuxDiskLight::Define(*stage, sdfLightPath);
      this->entities[_light.id()] = diskLight.GetPrim();
      diskLight.CreateColorAttr(pxr::VtValue(pxr::GfVec3f(
          _light.diffuse().r(), _light.diffuse().g(), _light.diffuse().b())));
      break;
    }
    case ignition::msgs::Light::DIRECTIONAL:
    {
      auto directionalLight =
          pxr::UsdLuxDistantLight::Define(*stage, sdfLightPath);
      this->entities[_light.id()] = directionalLight.GetPrim();
      directionalLight.CreateColorAttr(pxr::VtValue(pxr::GfVec3f(
          _light.diffuse().r(), _light.diffuse().g(), _light.diffuse().b())));
      break;
    }
    default:
      return false;
  }

  // This is a workaround to set the light's intensity attribute. Using the
  // UsdLuxLightAPI sets the light's "inputs:intensity" attribute, but isaac
  // sim reads the light's "intensity" attribute. Both inputs:intensity and
  // intensity are set to provide flexibility with other USD renderers
  const float usdLightIntensity =
      static_cast<float>(_light.intensity()) * 1000.0f;
  auto lightPrim = stage->GetPrimAtPath(sdfLightPath);
  lightPrim
      .CreateAttribute(pxr::TfToken("intensity"), pxr::SdfValueTypeNames->Float,
                       false)
      .Set(usdLightIntensity);

  return true;
}

//////////////////////////////////////////////////
bool Scene::Init()
{
  bool result;
  ignition::msgs::Empty req;
  ignition::msgs::Scene ignScene;
  if (!this->dataPtr->node->Request(
          "/world/" + this->dataPtr->worldName + "/scene/info", req, 5000,
          ignScene, result))
  {
    ignwarn << "Error requesting scene info, make sure the world ["
            << this->dataPtr->worldName
            << "] is available, ignition-omniverse will keep trying..."
            << std::endl;
    if (!this->dataPtr->node->Request(
            "/world/" + this->dataPtr->worldName + "/scene/info", req, -1,
            ignScene, result))
    {
      ignerr << "Error request scene info" << std::endl;
      return false;
    }
  }
  if (!this->dataPtr->UpdateScene(ignScene))
  {
    ignerr << "Failed to init scene" << std::endl;
    return false;
  }

  std::vector<std::string> topics;
  this->dataPtr->node->TopicList(topics);

  for (auto const &topic : topics)
  {
    if (topic.find("/joint_state") != std::string::npos)
    {
      if (!this->dataPtr->node->Subscribe(
        topic, &Scene::Implementation::CallbackJoint, this->dataPtr.Get()))
      {
        ignerr << "Error subscribing to topic [" << topic << "]" << std::endl;
        return false;
      }
      else
      {
        ignmsg << "Subscribed to topic: [joint_state]" << std::endl;
      }
    }
  }

  std::string topic = "/world/" + this->dataPtr->worldName + "/pose/info";
  // Subscribe to a topic by registering a callback.
  if (!this->dataPtr->node->Subscribe(
          topic, &Scene::Implementation::CallbackPoses, this->dataPtr.Get()))
  {
    ignerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }
  else
  {
    ignmsg << "Subscribed to topic: [" << topic << "]" << std::endl;
  }

  topic = "/world/" + this->dataPtr->worldName + "/scene/info";
  if (!this->dataPtr->node->Subscribe(
          topic, &Scene::Implementation::CallbackScene, this->dataPtr.Get()))
  {
    ignerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }
  else
  {
    ignmsg << "Subscribed to topic: [" << topic << "]" << std::endl;
  }

  topic = "/world/" + this->dataPtr->worldName + "/scene/deletion";
  if (!this->dataPtr->node->Subscribe(
          topic, &Scene::Implementation::CallbackSceneDeletion,
          this->dataPtr.Get()))
  {
    ignerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }
  else
  {
    ignmsg << "Subscribed to topic: [" << topic << "]" << std::endl;
  }

  return true;
}

//////////////////////////////////////////////////
void Scene::Save() { this->Stage()->Lock()->Save(); }

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void Scene::Implementation::CallbackPoses(const ignition::msgs::Pose_V &_msg)
{
  for (const auto &poseMsg : _msg.pose())
  {
    try
    {
      auto stage = this->stage->Lock();
      const auto &prim = this->entities.at(poseMsg.id());
      this->SetPose(pxr::UsdGeomXformCommonAPI(prim), poseMsg);
    }
    catch (const std::out_of_range &)
    {
      ignwarn << "Error updating pose, cannot find [" << poseMsg.name() << "]"
              << std::endl;
    }
  }
}

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void Scene::Implementation::CallbackJoint(const ignition::msgs::Model &_msg)
{
  this->UpdateModel(_msg);
}

//////////////////////////////////////////////////
void Scene::Implementation::CallbackScene(const ignition::msgs::Scene &_scene)
{
  this->UpdateScene(_scene);
}

//////////////////////////////////////////////////
void Scene::Implementation::CallbackSceneDeletion(
    const ignition::msgs::UInt32_V &_msg)
{
  for (const auto id : _msg.data())
  {
    try
    {
      auto stage = this->stage->Lock();
      const auto &prim = this->entities.at(id);
      stage->RemovePrim(prim.GetPath());
      ignmsg << "Removed [" << prim.GetPath() << "]" << std::endl;
    }
    catch (const std::out_of_range &)
    {
      ignwarn << "Failed to delete [" << id << "] (Unable to find node)"
              << std::endl;
    }
  }
}
}  // namespace omniverse
}  // namespace ignition
