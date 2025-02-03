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

#include <gz/msgs/uint32_v.pb.h>

#include <omni/connect/core/XformAlgo.h>

#include "Scene.hpp"
#include "Util.hpp"

#include "FUSDLayerNoticeListener.hpp"
#include "FUSDNoticeListener.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/math/Quaternion.hh>

#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdLux/diskLight.h>
#include <pxr/usd/usdLux/distantLight.h>
#include <pxr/usd/usdLux/sphereLight.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <pxr/base/tf/nullPtr.h>

using namespace std::chrono_literals;

namespace gz
{
namespace omniverse
{
class Scene::Implementation
{
 public:
  std::string worldName;
  pxr::UsdStageRefPtr stage;
  gz::transport::Node node;
  std::string stageDirUrl;
  std::unordered_map<uint32_t, pxr::UsdPrim> entities;
  std::unordered_map<std::string, uint32_t> entitiesByName;

  std::shared_ptr<FUSDLayerNoticeListener> USDLayerNoticeListener;
  std::shared_ptr<FUSDNoticeListener> USDNoticeListener;
  Simulator simulatorPoses = {Simulator::gz};
  std::string scopeName;
  gz::math::Pose3d scopePose;
  std::mutex stageMutex;

  bool UpdateSensors(const gz::msgs::Sensor &_sensor,
                    const std::string &_usdSensorPath);
  bool UpdateLights(const gz::msgs::Light &_light,
                    const std::string &_usdLightPath);
  bool UpdateScene(const gz::msgs::Scene &_scene);
  bool UpdateVisual(const gz::msgs::Visual &_visual,
                    const std::string &_usdPath);
  bool UpdateLink(const gz::msgs::Link &_link,
                  const std::string &_usdModelPath);
  bool UpdateJoint(const gz::msgs::Joint &_joint,
                   const std::string &_modelName);
  bool UpdateModel(const gz::msgs::Model &_model, const std::string& parent_path);
  void SetPose(const pxr::UsdGeomXformCommonAPI &_prim,
               const gz::math::Pose3d &_pose);
  void SetPose(const pxr::UsdGeomXformCommonAPI &_prim,
               const gz::msgs::Pose &_pose);
  void ResetPose(const pxr::UsdGeomXformCommonAPI &_prim);
  void SetScale(const pxr::UsdGeomXformCommonAPI &_xform,
                const gz::msgs::Vector3d &_scale);
  void ResetScale(const pxr::UsdGeomXformCommonAPI &_prim);
  void CallbackPoses(const gz::msgs::Pose_V &_msg);
  void CallbackJoint(const gz::msgs::Model &_msg);
  void CallbackScene(const gz::msgs::Scene &_scene);
  void CallbackSceneDeletion(const gz::msgs::UInt32_V &_msg);
};

//////////////////////////////////////////////////
Scene::Scene(
  const std::string &_worldName,
  const std::string &_stageUrl,
  pxr::UsdStageRefPtr stage,
  Simulator _simulatorPoses,
  bool use_live,
  const std::string &_scopeName,
  const gz::math::Pose3d &_scopePose)
    : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
  gzmsg << "Opened stage [" << _stageUrl << "]" << std::endl;
  this->dataPtr->worldName = _worldName;
  if (use_live) {
    this->dataPtr->stage = stage;
  } else {
    this->dataPtr->stage = pxr::UsdStage::Open(_stageUrl);
  }
  this->dataPtr->stageDirUrl = gz::common::parentPath(_stageUrl);

  this->dataPtr->simulatorPoses = _simulatorPoses;
  this->dataPtr->scopeName = _scopeName;
  this->dataPtr->scopePose = _scopePose;
}

// //////////////////////////////////////////////////
// std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> &Scene::Stage()
// {
//   return this->dataPtr->stage;
// }

//////////////////////////////////////////////////
std::mutex& Scene::Mutex() {
  return this->dataPtr->stageMutex;
}

//////////////////////////////////////////////////
void Scene::Implementation::SetPose(const pxr::UsdGeomXformCommonAPI &_prim,
                                    const gz::math::Pose3d &_pose)
{
  if (this->simulatorPoses == Simulator::gz)
  {
    if (_prim)
    {
      pxr::UsdGeomXformCommonAPI xformApi(_prim);
      xformApi.SetTranslate(pxr::GfVec3d(_pose.Pos().X(), _pose.Pos().Y(),
                                         _pose.Pos().Z()));
      xformApi.SetRotate(pxr::GfVec3f(
          gz::math::Angle(_pose.Rot().Roll()).Degree(),
          gz::math::Angle(_pose.Rot().Pitch()).Degree(),
          gz::math::Angle(_pose.Rot().Yaw()).Degree()),
        pxr::UsdGeomXformCommonAPI::RotationOrderXYZ);
    }
  }
}

//////////////////////////////////////////////////
void Scene::Implementation::SetPose(const pxr::UsdGeomXformCommonAPI &_prim,
                                    const gz::msgs::Pose &_pose)
{
  if (this->simulatorPoses == Simulator::gz)
  {
    if (_prim)
    {
      pxr::UsdGeomXformCommonAPI xformApi(_prim);
      const auto &pos = _pose.position();
      const auto &orient = _pose.orientation();
      gz::math::Quaterniond quat(orient.w(), orient.x(), orient.y(),
                                       orient.z());
      xformApi.SetTranslate(pxr::GfVec3d(pos.x(), pos.y(), pos.z()));
      xformApi.SetRotate(pxr::GfVec3f(
          gz::math::Angle(quat.Roll()).Degree(),
          gz::math::Angle(quat.Pitch()).Degree(),
          gz::math::Angle(quat.Yaw()).Degree()),
        pxr::UsdGeomXformCommonAPI::RotationOrderXYZ);
    }
  }
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
                                     const gz::msgs::Vector3d &_scale)
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
bool Scene::Implementation::UpdateVisual(const gz::msgs::Visual &_visual,
                                         const std::string &_usdLinkPath)
{
  auto stagePtr = this->stage;

  std::string visualName = _visual.name();
  std::string suffix = "_visual";
  std::size_t found = visualName.find("_visual");
  if (found != std::string::npos)
  {
    suffix = "";
  }

  std::string usdVisualPath = _usdLinkPath + "/" + validPath(_visual.name() + suffix);
  auto prim = stagePtr->GetPrimAtPath(pxr::SdfPath(usdVisualPath));
  if (prim)
    return true;

  auto usdVisualXform =
      pxr::UsdGeomXform::Define(stagePtr, pxr::SdfPath(usdVisualPath));
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
  this->entitiesByName[usdVisualXform.GetPrim().GetName()] = _visual.id();

  std::string usdGeomPath(usdVisualPath + "/geometry");
  const auto &geom = _visual.geometry();

  switch (geom.type())
  {
    case gz::msgs::Geometry::BOX:
    {
      auto usdCube =
          pxr::UsdGeomCube::Define(stagePtr, pxr::SdfPath(usdGeomPath));
      usdCube.CreateSizeAttr().Set(1.0);
      pxr::GfVec3f endPoint(0.5);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCube.CreateExtentAttr().Set(extentBounds);
      pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
      cubeXformAPI.SetScale(pxr::GfVec3f(
          geom.box().size().x(), geom.box().size().y(), geom.box().size().z()));
      if (!SetMaterial(usdCube, _visual, stagePtr, this->stageDirUrl))
      {
        gzwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    // TODO: Support cone
    // case gz::msgs::Geometry::CONE:
    case gz::msgs::Geometry::CYLINDER:
    {
      auto usdCylinder =
          pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath(usdGeomPath));
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
      if (!SetMaterial(usdCylinder, _visual, stagePtr, this->stageDirUrl))

      {
        gzwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case gz::msgs::Geometry::PLANE:
    {
      auto usdCube =
          pxr::UsdGeomCube::Define(stagePtr, pxr::SdfPath(usdGeomPath));
      usdCube.CreateSizeAttr().Set(1.0);
      pxr::GfVec3f endPoint(0.5);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(-1.0 * endPoint);
      extentBounds.push_back(endPoint);
      usdCube.CreateExtentAttr().Set(extentBounds);

      pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
      cubeXformAPI.SetScale(
          pxr::GfVec3f(geom.plane().size().x(), geom.plane().size().y(), 0.0025));
      if (!SetMaterial(usdCube, _visual, stagePtr, this->stageDirUrl))
      {
        gzwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case gz::msgs::Geometry::ELLIPSOID:
    {
      auto usdEllipsoid =
          pxr::UsdGeomSphere::Define(stagePtr, pxr::SdfPath(usdGeomPath));
      const auto maxRadii =
          gz::math::Vector3d(geom.ellipsoid().radii().x(),
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
      if (!SetMaterial(usdEllipsoid, _visual, stagePtr, this->stageDirUrl))
      {
        gzwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case gz::msgs::Geometry::SPHERE:
    {
      auto usdSphere =
          pxr::UsdGeomSphere::Define(stagePtr, pxr::SdfPath(usdGeomPath));
      double radius = geom.sphere().radius();
      usdSphere.CreateRadiusAttr().Set(radius);
      pxr::VtArray<pxr::GfVec3f> extentBounds;
      extentBounds.push_back(pxr::GfVec3f(-1.0 * radius));
      extentBounds.push_back(pxr::GfVec3f(radius));
      usdSphere.CreateExtentAttr().Set(extentBounds);
      if (!SetMaterial(usdSphere, _visual, stagePtr, this->stageDirUrl))
      {
        gzwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case gz::msgs::Geometry::CAPSULE:
    {
      auto usdCapsule =
          pxr::UsdGeomCapsule::Define(stagePtr, pxr::SdfPath(usdGeomPath));
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
      if (!SetMaterial(usdCapsule, _visual, stagePtr, this->stageDirUrl))
      {
        gzwarn << "Failed to set material" << std::endl;
      }
      break;
    }
    case gz::msgs::Geometry::MESH:
    {
      if (!UpdateMesh(geom.mesh(), usdGeomPath, stagePtr))
      {
        gzerr << "Failed to update visual [" << _visual.name() << "]"
               << std::endl;
        return false;
      }
      // Disable since Mesh geometry should include material. Enable again once this is merged with gz-usd.
      // if (!SetMaterial(usdMesh, _visual, stagePtr, this->stageDirUrl))
      // {
      //   gzerr << "Failed to update visual [" << _visual.name() << "]"
      //          << std::endl;
      //   return false;
      // }
      break;
    }
    default:
      gzerr << "Failed to update geometry (unsuported geometry type '"
             << _visual.type() << "')" << std::endl;
      return false;
  }

  // TODO(ahcorde): When usdphysics will be available in nv-usd we should
  // replace this code with pxr::UsdPhysicsCollisionAPI::Apply(geomPrim)
  pxr::TfToken appliedSchemaNamePhysicsCollisionAPI("PhysicsCollisionAPI");
  pxr::SdfPrimSpecHandle primSpec = pxr::SdfCreatePrimInLayer(
          stagePtr->GetEditTarget().GetLayer(),
          pxr::SdfPath(usdGeomPath));
  pxr::SdfTokenListOp listOpPanda;
  // Use ReplaceOperations to append in place.
  if (!listOpPanda.ReplaceOperations(pxr::SdfListOpTypeExplicit,
       0, 0, {appliedSchemaNamePhysicsCollisionAPI})) {
     gzerr << "Error Applying schema PhysicsCollisionAPI" << '\n';
  }
  primSpec->SetInfo(
    pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOpPanda));

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateLink(const gz::msgs::Link &_link,
                                       const std::string &_usdModelPath)
{
  auto stagePtr = this->stage;

  std::string linkName = _link.name();
  std::string suffix = "_link";
  std::size_t found = linkName.find("_link");
  if (found != std::string::npos)
  {
    suffix = "";
  }

  std::string usdLinkPath = _usdModelPath + "/" + validPath(_link.name());
  auto prim = stagePtr->GetPrimAtPath(pxr::SdfPath(usdLinkPath));
  if (prim)
  {
    return true;
  }
  else
  {
    usdLinkPath = _usdModelPath + "/" + validPath(_link.name() + suffix);
    prim = stagePtr->GetPrimAtPath(pxr::SdfPath(usdLinkPath));
    if (prim)
    {
      return true;
    }
  }

  auto xform = pxr::UsdGeomXform::Define(stagePtr, pxr::SdfPath(usdLinkPath));
  pxr::UsdGeomXformCommonAPI xformApi(xform);

  if (_link.has_pose())
  {
    this->SetPose(xformApi, _link.pose());
  }
  else
  {
    this->ResetPose(xformApi);
  }
  gzmsg << "Saved link: " << _link.name() << " with id " << _link.id() << std::endl;
  this->entities[_link.id()] = xform.GetPrim();
  this->entitiesByName[xform.GetPrim().GetName()] = _link.id();

  for (const auto &visual : _link.visual())
  {
    if (!this->UpdateVisual(visual, usdLinkPath))
    {
      gzerr << "Failed to update link [" << _link.name() << "]" << std::endl;
      return false;
    }
  }

  for (const auto &sensor : _link.sensor())
  {
    std::string usdSensorPath = usdLinkPath + "/" + validPath(sensor.name());
    if (!this->UpdateSensors(sensor, usdSensorPath))
    {
      gzerr << "Failed to add sensor [" << usdSensorPath << "]" << std::endl;
      return false;
    }
  }

  for (const auto &light : _link.light())
  {
    if (!this->UpdateLights(light, usdLinkPath + "/" + validPath(light.name())))
    {
      gzerr << "Failed to add light [" << usdLinkPath + "/" + validPath(light.name())
             << "]" << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateJoint(
  const gz::msgs::Joint &_joint, const std::string &_modelName)
{
  auto stagePtr = this->stage;
  auto jointUSD =
      stagePtr->GetPrimAtPath(pxr::SdfPath("/" + worldName + "/" + validPath(_joint.name())));
  // TODO(ahcorde): This code is duplicated in the sdformat converter.
  if (!jointUSD)
  {
    jointUSD =
        stagePtr->GetPrimAtPath(
          pxr::SdfPath(
            "/" + worldName + "/" + validPath(_modelName) + "/" + validPath(_joint.name())));
    if (!jointUSD)
    {
      switch (_joint.type())
      {
        case gz::msgs::Joint::FIXED:
        {
          pxr::TfToken usdPrimTypeName("PhysicsFixedJoint");
          auto jointFixedUSD = stagePtr->DefinePrim(
            pxr::SdfPath("/" + this->worldName + "/" + validPath(_joint.name())),
            usdPrimTypeName);

          auto body0 = jointFixedUSD.CreateRelationship(
            pxr::TfToken("physics:body0"), false);
          body0.AddTarget(pxr::SdfPath(
            "/" + this->worldName + "/" + validPath(_joint.parent())));
          auto body1 = jointFixedUSD.CreateRelationship(
            pxr::TfToken("physics:body1"), false);
          body1.AddTarget(pxr::SdfPath(
            "/" + this->worldName + "/" + validPath(_joint.child())));

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
        case gz::msgs::Joint::REVOLUTE:
        {
          igndbg << "Creating a revolute joint" << '\n';

          pxr::TfToken usdPrimTypeName("PhysicsRevoluteJoint");
          auto revoluteJointUSD = stagePtr->DefinePrim(
            pxr::SdfPath("/" + this->worldName + "/" + validPath(_joint.name())),
            usdPrimTypeName);

          igndbg << "\tParent "
                 << "/" + this->worldName + "/" + validPath(_joint.parent()) << '\n';
          igndbg << "\tchild "
                 << "/" + this->worldName + "/" + validPath(_joint.child()) << '\n';

          pxr::TfTokenVector identifiersBody0 =
              {pxr::TfToken("physics"), pxr::TfToken("body0")};

          if (pxr::UsdRelationship body0 = revoluteJointUSD.CreateRelationship(
            pxr::TfToken(pxr::SdfPath::JoinIdentifier(identifiersBody0)), false))
          {
            body0.AddTarget(
              pxr::SdfPath("/" + this->worldName + "/panda/" + validPath(_joint.parent())),
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
              pxr::SdfPath("/" + this->worldName + "/panda/" + validPath(_joint.child())),
              pxr::UsdListPositionFrontOfAppendList);
          }
          else
          {
            igndbg << "Not able to create UsdRelationship for body1" << '\n';
          }

          gz::math::Vector3i axis(
            _joint.axis1().xyz().x(),
            _joint.axis1().xyz().y(),
            _joint.axis1().xyz().z());

          if (axis == gz::math::Vector3i(1, 0, 0))
          {
            revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
              pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("X"));
          }
          else if (axis == gz::math::Vector3i(0, 1, 0))
          {
            revoluteJointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
              pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("Y"));
          }
          else if (axis == gz::math::Vector3i(0, 0, 1))
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
            stagePtr->GetEditTarget().GetLayer(),
            pxr::SdfPath("/" + this->worldName + "/panda"));
          pxr::SdfTokenListOp listOpPanda;
          // Use ReplaceOperations to append in place.
          if (!listOpPanda.ReplaceOperations(
            pxr::SdfListOpTypeExplicit,
            0,
            0,
            {appliedSchemaNamePhysicsArticulationRootAPI,
             appliedSchemaNamePhysxArticulationAPI})) {
            gzerr << "Not able to setup the schema PhysxArticulationAPI "
                   << "and PhysicsArticulationRootAPI\n";
          }
          primSpecPanda->SetInfo(
            pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOpPanda));

          pxr::TfToken appliedSchemaName("PhysicsDriveAPI:angular");
          pxr::SdfPrimSpecHandle primSpec = pxr::SdfCreatePrimInLayer(
            stagePtr->GetEditTarget().GetLayer(),
            pxr::SdfPath("/" + this->worldName + "/" + validPath(_joint.name())));
          pxr::SdfTokenListOp listOp;

          // Use ReplaceOperations to append in place.
          if (!listOp.ReplaceOperations(pxr::SdfListOpTypeExplicit,
                  0, 0, {appliedSchemaName})) {
            gzerr << "Not able to setup the schema PhysicsDriveAPI\n";
          }

          primSpec->SetInfo(
            pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOp));
          break;
        }
        default:
          return false;
      }
    }
  }
  auto attrTargetPos = jointUSD.GetAttribute(
      pxr::TfToken("drive:angular:physics:targetPosition"));
  if (attrTargetPos)
  {
    attrTargetPos.Set(pxr::VtValue(
        static_cast<float>(
          gz::math::Angle(_joint.axis1().position()).Degree())));
  }
  else
  {
    jointUSD.CreateAttribute(
      pxr::TfToken("drive:angular:physics:targetPosition"),
      pxr::SdfValueTypeNames->Float, false).Set(
        static_cast<float>(
          gz::math::Angle(_joint.axis1().position()).Degree()));
  }
  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateModel(const gz::msgs::Model &_model, const std::string& parent_path)
{
  auto stagePtr = this->stage;

  std::string modelName = _model.name();

  if (modelName.empty())
    return true;

  std::string usdModelPath = "/" + worldName + "/" + validPath(modelName);
  if (!parent_path.empty()) {
    usdModelPath = parent_path + "/" + validPath(modelName);
  }
  gzmsg << "Updating model " << modelName << " at path " << usdModelPath << std::endl;

  auto range = pxr::UsdPrimRange::Stage(stagePtr);
  for (auto const &primRange : range)
  {
    if (primRange.GetName().GetString() == validPath(modelName))
    {
      gzwarn << "The model [" << _model.name() << "] is already available"
              << " in Isaac Sim" << std::endl;

      auto prim = stagePtr->GetPrimAtPath(
            pxr::SdfPath(usdModelPath));
      if (prim)
      {
        gzmsg << "Found existing prim for model " << _model.name() << std::endl;
        this->entities[_model.id()] = prim;
        this->entitiesByName[prim.GetName()] = _model.id();

        for (const auto &link : _model.link())
        {
          std::string linkName = link.name();
          std::string suffix = "_link";
          std::size_t found = linkName.find("_link");
          if (found != std::string::npos)
          {
            suffix = "";
          }
          std::string usdLinkPath = usdModelPath + "/" + validPath(linkName + suffix);
          auto linkPrim = stagePtr->GetPrimAtPath(
                pxr::SdfPath(usdLinkPath));
          if (!linkPrim)
          {
            usdLinkPath = usdModelPath + "/" + validPath(linkName);
            linkPrim = stagePtr->GetPrimAtPath(
                  pxr::SdfPath(usdLinkPath));
          }
          if (linkPrim)
          {
            this->entities[link.id()] = linkPrim;
            this->entitiesByName[linkPrim.GetName()] = link.id();
            gzmsg << "Adding link " << linkName << " - " << link.id() << " for model " << _model.name() << std::endl;
            for (const auto &visual : link.visual())
            {
              std::string visualName = visual.name();
              std::string suffix = "_visual";
              found = visualName.find("_visual");
              if (found != std::string::npos)
              {
                suffix = "";
              }
              std::string usdvisualPath =
                usdLinkPath + "/" + validPath(visualName + suffix);
              auto visualPrim = stagePtr->GetPrimAtPath(
                    pxr::SdfPath(usdvisualPath));
              if (visualPrim)
              {
                this->entities[visual.id()] = visualPrim;
                this->entitiesByName[visualPrim.GetName()] = visual.id();
              }
              else
              {
                usdvisualPath =
                  usdLinkPath + "/" + validPath(visualName);
                visualPrim = stagePtr->GetPrimAtPath(
                      pxr::SdfPath(usdvisualPath));
                if (visualPrim)
                {
                  this->entities[visual.id()] = visualPrim;
                  this->entitiesByName[visualPrim.GetName()] = visual.id();
                }
              }
            }
            for (const auto &light : link.light())
            {
              std::string usdLightPath =
                usdLinkPath + "/" + validPath(light.name());
              auto lightPrim = stagePtr->GetPrimAtPath(
                    pxr::SdfPath(usdLightPath));
              if (lightPrim)
              {
                this->entities[light.id()] = lightPrim;
                this->entitiesByName[lightPrim.GetName()] = light.id();
              }
            }
          }
        }
      }
    }
  }

  this->entitiesByName[modelName] = _model.id();

  auto xform = pxr::UsdGeomXform::Define(stagePtr, pxr::SdfPath(usdModelPath));
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
      gzerr << "Failed to update link for model [" << modelName << "]" << std::endl;
      return false;
    }
  }

  for (const auto &child_model : _model.model())
  {
    gzmsg << "Updating child model " << modelName << std::endl;
    if (!this->UpdateModel(child_model, usdModelPath))
    {
      gzerr << "Failed to update child model for model [" << modelName << "]" << std::endl;
      return false;
    }
  }

  for (const auto &joint : _model.joint())
  {
    if (!this->UpdateJoint(joint, _model.name()))
    {
      gzerr << "Failed to update joint for model [" << modelName << "]" << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateScene(const gz::msgs::Scene &_scene)
{
  std::string usdScopePath;
  if (!this->scopeName.empty())
  {
    // Check if scope prim is already defined. If not, define it.
    auto stagePtr = this->stage;

    usdScopePath = "/" + validPath(this->scopeName);
    auto prim = stagePtr->GetPrimAtPath(pxr::SdfPath(usdScopePath));
    if (!prim)
    {
      auto usdScopeXform =
        pxr::UsdGeomXform::Define(stagePtr, pxr::SdfPath(usdScopePath));
      pxr::UsdGeomXformCommonAPI xformApi(usdScopeXform);
      this->SetPose(xformApi, this->scopePose);
    }
  }

  std::string basePath = usdScopePath + "/" + this->worldName;
  for (const auto &model : _scene.model())
  {
    if (!this->UpdateModel(model, basePath))
    {
      gzerr << "Failed to add model [" << model.name() << "]" << std::endl;
      return false;
    }
    igndbg << "added model [" << model.name() << "]" << std::endl;
  }

  for (const auto &light : _scene.light())
  {
    if (!this->UpdateLights(light, basePath + "/" + light.name()))
    {
      gzerr << "Failed to add light [" << light.name() << "]" << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////
bool Scene::Implementation::UpdateSensors(const gz::msgs::Sensor &_sensor,
                   const std::string &_usdSensorPath)
{
  auto stagePtr = this->stage;

  // TODO(ahcorde): This code is duplicated in the USD converter (sdformat)
  if (_sensor.type() == "camera")
  {
    auto usdCamera = pxr::UsdGeomCamera::Define(
      stagePtr, pxr::SdfPath(_usdSensorPath));

    // TODO(ahcorde): The default value in USD is 50, but something more
    // similar to gz Gazebo is 40.
    usdCamera.CreateFocalLengthAttr().Set(
        static_cast<float>(52.0f));

    usdCamera.CreateClippingRangeAttr().Set(pxr::GfVec2f(
          static_cast<float>(_sensor.camera().near_clip()),
          static_cast<float>(_sensor.camera().far_clip())));
    usdCamera.CreateHorizontalApertureAttr().Set(
      static_cast<float>(
        _sensor.camera().horizontal_fov() * 180.0f / GZ_PI));

    gz::math::Pose3d poseCameraYUp(0, 0, 0, GZ_PI_2, 0, -GZ_PI_2);
    gz::math::Quaterniond q(
      _sensor.pose().orientation().w(),
      _sensor.pose().orientation().x(),
      _sensor.pose().orientation().y(),
      _sensor.pose().orientation().z());

    gz::math::Pose3d poseCamera(
      _sensor.pose().position().x(),
      _sensor.pose().position().y(),
      _sensor.pose().position().z(),
      q.Roll() * 180.0 / GZ_PI,
      q.Pitch() * 180.0 / GZ_PI,
      q.Yaw() * 180. / GZ_PI);

    poseCamera = poseCamera * poseCameraYUp;

    usdCamera.AddTranslateOp(pxr::UsdGeomXformOp::Precision::PrecisionDouble)
      .Set(
        pxr::GfVec3d(
          poseCamera.Pos().X(),
          poseCamera.Pos().Y(),
          poseCamera.Pos().Z()));

    usdCamera.AddRotateXYZOp(pxr::UsdGeomXformOp::Precision::PrecisionDouble)
     .Set(
        pxr::GfVec3d(
          poseCamera.Rot().Roll() * 180.0 / GZ_PI,
          poseCamera.Rot().Pitch() * 180.0 / GZ_PI,
          poseCamera.Rot().Yaw() * 180. / GZ_PI));
  }
  else if (_sensor.type() == "gpu_lidar")
  {
    pxr::UsdGeomXform::Define(
      stagePtr, pxr::SdfPath(_usdSensorPath));
    auto lidarPrim = stagePtr->GetPrimAtPath(
          pxr::SdfPath(_usdSensorPath));
    lidarPrim.SetTypeName(pxr::TfToken("Lidar"));

    lidarPrim.CreateAttribute(pxr::TfToken("minRange"),
        pxr::SdfValueTypeNames->Float, false).Set(
          static_cast<float>(_sensor.lidar().range_min()));
    lidarPrim.CreateAttribute(pxr::TfToken("maxRange"),
        pxr::SdfValueTypeNames->Float, false).Set(
          static_cast<float>(_sensor.lidar().range_max()));
    const auto horizontalFov = _sensor.lidar().horizontal_max_angle() -
      _sensor.lidar().horizontal_min_angle();
    // TODO(adlarkin) double check if these FOV calculations are correct
    lidarPrim.CreateAttribute(pxr::TfToken("horizontalFov"),
        pxr::SdfValueTypeNames->Float, false).Set(
          static_cast<float>(horizontalFov * 180.0f / GZ_PI));
    const auto verticalFov = _sensor.lidar().vertical_max_angle() -
      _sensor.lidar().vertical_min_angle();
    lidarPrim.CreateAttribute(pxr::TfToken("verticalFov"),
        pxr::SdfValueTypeNames->Float, false).Set(
          static_cast<float>(verticalFov * 180.0f / GZ_PI));
    lidarPrim.CreateAttribute(pxr::TfToken("horizontalResolution"),
        pxr::SdfValueTypeNames->Float, false).Set(
          static_cast<float>(_sensor.lidar().horizontal_resolution()));
    lidarPrim.CreateAttribute(pxr::TfToken("verticalResolution"),
        pxr::SdfValueTypeNames->Float, false).Set(
          static_cast<float>(_sensor.lidar().vertical_resolution()));
  }
  else
  {
    gzwarn << "This kind of sensor [" << _sensor.type()
            << "] is not supported" << std::endl;
    return true;
  }
  return true;
}
//////////////////////////////////////////////////
bool Scene::Implementation::UpdateLights(const gz::msgs::Light &_light,
                                       const std::string &_usdLightPath)
{
  // TODO: We can probably re-use code from sdformat

  auto stagePtr = this->stage;

  const pxr::SdfPath sdfLightPath(_usdLightPath);
  switch (_light.type())
  {
    case gz::msgs::Light::POINT:
    {
      auto pointLight = pxr::UsdLuxSphereLight::Define(stagePtr, sdfLightPath);
      pointLight.CreateTreatAsPointAttr().Set(true);
      this->entities[_light.id()] = pointLight.GetPrim();
      this->entitiesByName[pointLight.GetPrim().GetName()] = _light.id();
      pointLight.CreateRadiusAttr(pxr::VtValue(0.1f));
      pointLight.CreateColorAttr(pxr::VtValue(pxr::GfVec3f(
          _light.diffuse().r(), _light.diffuse().g(), _light.diffuse().b())));
      break;
    }
    case gz::msgs::Light::SPOT:
    {
      auto diskLight = pxr::UsdLuxDiskLight::Define(stagePtr, sdfLightPath);
      this->entities[_light.id()] = diskLight.GetPrim();
      this->entitiesByName[diskLight.GetPrim().GetName()] = _light.id();
      diskLight.CreateColorAttr(pxr::VtValue(pxr::GfVec3f(
          _light.diffuse().r(), _light.diffuse().g(), _light.diffuse().b())));
      break;
    }
    case gz::msgs::Light::DIRECTIONAL:
    {
      auto directionalLight =
          pxr::UsdLuxDistantLight::Define(stagePtr, sdfLightPath);
      this->entities[_light.id()] = directionalLight.GetPrim();
      this->entitiesByName[directionalLight.GetPrim().GetName()] = _light.id();
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
  auto lightPrim = stagePtr->GetPrimAtPath(sdfLightPath);
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
  gz::msgs::Empty req;
  gz::msgs::Scene ignScene;
  if (!this->dataPtr->node.Request(
          "/world/" + this->dataPtr->worldName + "/scene/info", req, 5000,
          ignScene, result))
  {
    gzwarn << "Error requesting scene info, make sure the world ["
            << this->dataPtr->worldName
            << "] is available, gz-omniverse will keep trying..."
            << std::endl;
    if (!this->dataPtr->node.Request(
            "/world/" + this->dataPtr->worldName + "/scene/info", req, -1,
            ignScene, result))
    {
      gzerr << "Error request scene info" << std::endl;
      return false;
    }
  }

  if (!this->dataPtr->UpdateScene(ignScene))
  {
    gzerr << "Failed to init scene" << std::endl;
    return false;
  }

  std::vector<std::string> topics;
  this->dataPtr->node.TopicList(topics);

  for (auto const &topic : topics)
  {
    if (topic.find("/joint_state") != std::string::npos)
    {
      if (!this->dataPtr->node.Subscribe(
        topic, &Scene::Implementation::CallbackJoint, this->dataPtr.get()))
      {
        gzerr << "Error subscribing to topic [" << topic << "]" << std::endl;
        return false;
      }
      else
      {
        gzmsg << "Subscribed to topic: [joint_state]" << std::endl;
      }
    }
  }

  std::string topic = "/world/" + this->dataPtr->worldName + "/pose/info";
  // Subscribe to a topic by registering a callback.
  if (!this->dataPtr->node.Subscribe(
          topic, &Scene::Implementation::CallbackPoses, this->dataPtr.get()))
  {
    std::cout << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }
  else
  {
    gzmsg << "Subscribed to topic: [" << topic << "]" << std::endl;
  }

  topic = "/world/" + this->dataPtr->worldName + "/scene/info";
  if (!this->dataPtr->node.Subscribe(
          topic, &Scene::Implementation::CallbackScene, this->dataPtr.get()))
  {
    gzerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }
  else
  {
    gzmsg << "Subscribed to topic: [" << topic << "]" << std::endl;
  }

  topic = "/world/" + this->dataPtr->worldName + "/scene/deletion";
  if (!this->dataPtr->node.Subscribe(
          topic, &Scene::Implementation::CallbackSceneDeletion,
          this->dataPtr.get()))
  {
    gzerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }
  else
  {
    gzmsg << "Subscribed to topic: [" << topic << "]" << std::endl;
  }

  // TODO: Re-enable listeners
  // this->dataPtr->USDLayerNoticeListener =
  //   std::make_shared<FUSDLayerNoticeListener>(
  //     this->dataPtr->stage,
  //     this->dataPtr->worldName);
  // auto LayerReloadKey = pxr::TfNotice::Register(
  //     pxr::TfCreateWeakPtr(this->dataPtr->USDLayerNoticeListener.get()),
  //     &FUSDLayerNoticeListener::HandleGlobalLayerReload);
  // auto LayerChangeKey = pxr::TfNotice::Register(
  //     pxr::TfCreateWeakPtr(this->dataPtr->USDLayerNoticeListener.get()),
  //     &FUSDLayerNoticeListener::HandleRootOrSubLayerChange,
  //     this->dataPtr->stage->Lock()->GetRootLayer());

  // this->dataPtr->USDNoticeListener = std::make_shared<FUSDNoticeListener>(
  //   this->dataPtr->stage,
  //   this->dataPtr->worldName,
  //   this->dataPtr->simulatorPoses,
  //   this->dataPtr->entitiesByName);
  // auto USDNoticeKey = pxr::TfNotice::Register(
  //     pxr::TfCreateWeakPtr(this->dataPtr->USDNoticeListener.get()),
  //     &FUSDNoticeListener::Handle);
  return true;
}

//////////////////////////////////////////////////
void Scene::Save() { this->dataPtr->stage->Save(); }

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void Scene::Implementation::CallbackPoses(const gz::msgs::Pose_V &_msg)
{
  std::lock_guard<std::mutex> l(this->stageMutex);

  // gzmsg << "Updating poses" << std::endl;
  for (const auto &_pose : _msg.pose())
  {
    try
    {
      // gzmsg << "Setting pose for " << _pose.name() << " - " << _pose.id() << std::endl;
      // TODO: pxr::UsdEditContext to take stage pointer and layer (instead of taking the entire stage)
      auto stagePtr = this->stage;
      const auto &prim = this->entities.at(_pose.id());
      if (prim)
      {
        this->SetPose(pxr::UsdGeomXformCommonAPI(prim), _pose);
        // pxr::GfVec3d position(0);
        // pxr::GfVec3d pivot(0);
        // pxr::GfVec3f rotXYZ(0);
        // omni::connect::core::RotationOrder rotationOrder;
        // pxr::GfVec3f scale(1);
        // const auto &pos = _pose.position();
        // const auto &orient = _pose.orientation();
        // gz::math::Quaterniond quat(orient.w(), orient.x(), orient.y(), orient.z());

        // omni::connect::core::getLocalTransformComponents(prim, position, pivot, rotXYZ, rotationOrder, scale);
        // omni::connect::core::setLocalTransform(
        //   prim,
        //   pxr::GfVec3d(pos.x(), pos.y(), pos.z()),
        //   pivot,
        //   pxr::GfVec3f(
        //     gz::math::Angle(quat.Roll()).Degree(),
        //     gz::math::Angle(quat.Pitch()).Degree(),
        //     gz::math::Angle(quat.Yaw()).Degree()),
        //   rotationOrder,
        //   scale
        // );
      }
    }
    catch (const std::out_of_range &)
    {
      gzwarn << "Error updating pose, cannot find [" << _pose.name() << " - " << _pose.id() << "]"
              << std::endl;
    }
  }
}

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void Scene::Implementation::CallbackJoint(const gz::msgs::Model &_msg)
{
  // this->UpdateModel(_msg);
  for (const auto &joint : _msg.joint())
  {
    if (!this->UpdateJoint(joint, _msg.name()))
    {
      gzerr << "Failed to update model [" << _msg.name() << "]" << std::endl;
      return;
    }
  }
}

//////////////////////////////////////////////////
void Scene::Implementation::CallbackScene(const gz::msgs::Scene &_scene)
{
  this->UpdateScene(_scene);
}

//////////////////////////////////////////////////
void Scene::Implementation::CallbackSceneDeletion(
    const gz::msgs::UInt32_V &_msg)
{
  for (const auto id : _msg.data())
  {
    try
    {
      auto stagePtr = this->stage;
      const auto &prim = this->entities.at(id);
      std::string primName = prim.GetName();
      stagePtr->RemovePrim(prim.GetPath());
      gzmsg << "Removed [" << prim.GetPath() << "]" << std::endl;
      this->entities.erase(id);
      this->entitiesByName.erase(primName);
    }
    catch (const std::out_of_range &)
    {
      gzwarn << "Failed to delete [" << id << "] (Unable to find node)"
              << std::endl;
    }
  }
}
}  // namespace omniverse
}  // namespace gz
