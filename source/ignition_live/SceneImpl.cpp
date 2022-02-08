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

#include "SceneImpl.hpp"

#include <algorithm>
#include <chrono>  // std::chrono::seconds
#include <string>
#include <thread>  // std::this_thread::sleep_for
#include <vector>

#include <ignition/math/Quaternion.hh>
#include <ignition/math/Pose3.hh>

#include <pxr/usd/usdGeom/xform.h>

#include "IgnitionVisual.hpp"

using namespace std::chrono_literals;

namespace ignition
{
namespace omniverse
{
//////////////////////////////////////////////////
SceneImpl::SceneImpl(const std::string &_worldName, pxr::UsdStageRefPtr _stage)
{
  this->worldName = _worldName;
  this->stage = _stage;
}

//////////////////////////////////////////////////
SceneImpl::SharedPtr SceneImpl::Make(const std::string &_worldName,
                                     pxr::UsdStageRefPtr _stage)
{
  auto sp = std::make_shared<SceneImpl>(_worldName, _stage);
  sp->Init();
  return sp;
}

//////////////////////////////////////////////////
pxr::UsdPrim SceneImpl::GetPrimAtPath(const std::string &_path)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  auto prim = this->stage->GetPrimAtPath(pxr::SdfPath(_path));
  return prim;
}

//////////////////////////////////////////////////
void SceneImpl::SaveStage()
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  this->stage->Save();
}

//////////////////////////////////////////////////
pxr::UsdGeomCapsule SceneImpl::CreateCapsule(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomCapsule::Define(this->stage, pxr::SdfPath(_name));
}

//////////////////////////////////////////////////
pxr::UsdGeomSphere SceneImpl::CreateSphere(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomSphere::Define(this->stage, pxr::SdfPath(_name));
}

//////////////////////////////////////////////////
pxr::UsdGeomCube SceneImpl::CreateCube(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomCube::Define(this->stage, pxr::SdfPath(_name));
}

//////////////////////////////////////////////////
pxr::UsdGeomSphere SceneImpl::CreateEllipsoid(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomSphere::Define(this->stage, pxr::SdfPath(_name));
}

//////////////////////////////////////////////////
pxr::UsdGeomCylinder SceneImpl::CreateCylinder(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomCylinder::Define(this->stage, pxr::SdfPath(_name));
}

//////////////////////////////////////////////////
pxr::UsdShadeMaterial SceneImpl::CreateMaterial(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdShadeMaterial::Define(this->stage, pxr::SdfPath(_name));
}

//////////////////////////////////////////////////
pxr::UsdShadeShader SceneImpl::CreateShader(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdShadeShader::Define(this->stage, pxr::SdfPath(_name));
}

pxr::UsdGeomXform SceneImpl::CreateXform(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomXform::Define(this->stage, pxr::SdfPath(_name));
}

pxr::UsdGeomMesh SceneImpl::CreateMesh(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  return pxr::UsdGeomMesh::Define(this->stage, pxr::SdfPath(_name));
}

pxr::UsdPrim SceneImpl::CreateFixedJoint(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  pxr::TfToken usdPrimTypeName("PhysicsFixedJoint");
  return this->stage->DefinePrim(
    pxr::SdfPath(_name),
    usdPrimTypeName);
}

pxr::UsdPrim SceneImpl::CreateRevoluteJoint(const std::string &_name)
{
  std::unique_lock<std::mutex> lkStage(mutexStage);
  pxr::TfToken usdPrimTypeName("PhysicsRevoluteJoint");
  return this->stage->DefinePrim(
    pxr::SdfPath(_name),
    usdPrimTypeName);
}

//////////////////////////////////////////////////
bool SceneImpl::Init()
{
  modelThread = std::make_shared<std::thread>(&SceneImpl::modelWorker, this);

  std::vector<std::string> topics;
  node.TopicList(topics);

  for (auto const &topic : topics)
  {
    if (topic.find("/joint_state") != std::string::npos)
    {
      if (!node.Subscribe(topic, &SceneImpl::CallbackJoint, this))
      {
        std::cerr << "Error subscribing to topic [" << topic << "]"
                  << std::endl;
        return false;
      }
      else
      {
        std::cout << "Subscribed to topic: [" << topic << "]" << std::endl;
      }
    }
  }

  std::string topic = "/world/" + worldName + "/pose/info";
  // Subscribe to a topic by registering a callback.
  if (!node.Subscribe(topic, &SceneImpl::CallbackPoses, this))
  {
    std::cerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return false;
  }

  return true;
}

//////////////////////////////////////////////////
std::unordered_map<std::string, IgnitionModel> SceneImpl::GetModels()
{
  std::unordered_map<std::string, IgnitionModel> result;
  std::unique_lock<std::mutex> lkPose(poseMutex);
  result = this->models;
  return result;
}

void SceneImpl::modelWorker()
{
  auto printvector = [](const auto &v)
  {
    std::cout << "{ ";
    for (auto i : v) std::cout << i << ' ';
    std::cout << "} "
              << "\n";
  };

  while (true)
  {
    std::set<std::string> modelNames;

    auto modelsTmp = GetModels();

    for (auto &model : modelsTmp)
    {
      modelNames.insert(model.first);
    }

    // Prepare the input parameters.
    ignition::msgs::Empty req;
    ignition::msgs::Scene rep;
    bool result;
    unsigned int timeout = 5000;
    bool executed = node.Request("/world/" + worldName + "/scene/info", req,
                                 timeout, rep, result);
    if (executed)
    {
      std::set<std::string> modelNamesReceived;

      for (auto &model : rep.model())
      {
        modelNamesReceived.insert(model.name());
      }

      std::vector<std::string> removed;
      std::vector<std::string> added;

      std::set_difference(modelNamesReceived.begin(), modelNamesReceived.end(),
                          modelNames.begin(), modelNames.end(),
                          std::inserter(added, added.begin()));

      std::set_difference(modelNames.begin(), modelNames.end(),
                          modelNamesReceived.begin(), modelNamesReceived.end(),
                          std::inserter(removed, removed.begin()));

      if (added.size() > 0)
      {
        std::cerr << "added " << '\n';
        printvector(added);
      }

      if (removed.size() > 0 && modelNames.size() > 0)
      {
        std::cerr << "removed " << '\n';
        printvector(removed);
        for (auto &removeModelName : removed)
        {
          auto it = modelsTmp.find(removeModelName);
          if (it != modelsTmp.end())
          {
            {
              std::unique_lock<std::mutex> lkStage(mutexStage);
              this->stage->RemovePrim(
                  pxr::SdfPath("/" + worldName + "/" + removeModelName));
            }
            {
              std::cerr << "removed: " << removeModelName << '\n';
              std::unique_lock<std::mutex> lkPose(poseMutex);
              this->models.erase(removeModelName);
            }
          }
        }
      }

      for (auto &model : rep.model())
      {
        auto it = modelsTmp.find(model.name());
        if (it == modelsTmp.end())
        {
          IgnitionModel ignitionModel;
          // ignitionModel.visuals.emplace_back(ignitionVisual);
          ignitionModel.pose = ignition::math::Pose3d(
              model.pose().position().x(), model.pose().position().y(),
              model.pose().position().z(), model.pose().orientation().w(),
              model.pose().orientation().x(), model.pose().orientation().y(),
              model.pose().orientation().z());
          ignitionModel.id = model.id();
          std::string sdfModelPath =
              std::string("/") + worldName + "/" + model.name();

          auto modelPrim = this->GetPrimAtPath(sdfModelPath);
          if (modelPrim)
          {
            std::cerr << "Model [" << model.name()
                      << "] already available in the scene" << '\n';
            {
              std::unique_lock<std::mutex> lkPose(poseMutex);
              this->models.insert({model.name(), ignitionModel});
            }
            continue;
          }

          auto usdModelXform = pxr::UsdGeomXform::Define(
              this->stage, pxr::SdfPath(sdfModelPath));
          for (auto &link : model.link())
          {
            std::string sdfLinkPath = sdfModelPath + "/" + link.name();

            auto usdLinkXform = pxr::UsdGeomXform::Define(
                this->stage, pxr::SdfPath(sdfLinkPath));

            ignition::math::Quaterniond q(
              link.pose().orientation().w(),
              link.pose().orientation().x(),
              link.pose().orientation().y(),
              link.pose().orientation().z());

            usdLinkXform.AddTranslateOp(
              pxr::UsdGeomXformOp::Precision::PrecisionDouble)
                .Set(
                  pxr::GfVec3d(
                    link.pose().position().x(),
                    link.pose().position().y(),
                    link.pose().position().z()));

            usdLinkXform.AddRotateXYZOp(
              pxr::UsdGeomXformOp::Precision::PrecisionDouble)
                .Set(
                  pxr::GfVec3d(
                    q.Roll() * 180 /3.1416,
                    q.Pitch() * 180 /3.1416,
                    q.Yaw() * 180 /3.1416));

            for (auto &visual : link.visual())
            {
              auto scene = this->SharedFromThis();
              auto ignitionVisual =
                  IgnitionVisual::Make(visual.id(), visual.name(), scene);
              ignitionVisual->AttachGeometry(visual, sdfLinkPath);

              {
                std::unique_lock<std::mutex> lkPose(poseMutex);
                this->models.insert({model.name(), ignitionModel});
              }
            }
          }
        }
      }
    }
    std::this_thread::sleep_for(20ms);
  }
}

//////////////////////////////////////////////////
bool SceneImpl::SetModelPose(const std::string &_name,
                             const ignition::math::Pose3d &_pose)
{
  std::unique_lock<std::mutex> lkPose(poseMutex);
  auto it = models.find(_name);
  if (it != models.end())
  {
    it->second.pose = _pose;
    it->second.pose.Correct();
    return true;
  }
  return false;
}

bool SceneImpl::RemoveModel(const std::string &_name)
{
  return static_cast<bool>(this->models.erase(_name));
}

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void SceneImpl::CallbackPoses(const ignition::msgs::Pose_V &_msg)
{
  std::unique_lock<std::mutex> lkPose(poseMutex);

  for (int i = 0; i < _msg.pose().size(); ++i)
  {
    const auto &poseMsg = _msg.pose(i);
    auto it = models.find(poseMsg.name());
    if (it != models.end())
    {
      it->second.pose = ignition::math::Pose3d(
          poseMsg.position().x(), poseMsg.position().y(),
          poseMsg.position().z(), poseMsg.orientation().w(),
          poseMsg.orientation().x(), poseMsg.orientation().y(),
          poseMsg.orientation().z());
      it->second.id = poseMsg.id();
      it->second.pose.Correct();
    }
  }
}

bool SceneImpl::ParseJoint(const ignition::msgs::Joint &_msg)
{
  switch (_msg.type())
  {
    case ignition::msgs::Joint::FIXED:
    {
      auto jointUSD = this->CreateFixedJoint("/" + _msg.name());

      auto body0 = jointUSD.CreateRelationship(pxr::TfToken("physics:body0"), false);
      body0.AddTarget(pxr::SdfPath("/" + this->worldName + "/" + _msg.parent()));
      auto body1 = jointUSD.CreateRelationship(pxr::TfToken("physics:body1"), false);
      body1.AddTarget(pxr::SdfPath("/" + this->worldName + "/" + _msg.child()));

      jointUSD.CreateAttribute(pxr::TfToken("physics:localPos1"),
              pxr::SdfValueTypeNames->Point3fArray, false).Set(
                pxr::GfVec3f(0, 0, 0));

      jointUSD.CreateAttribute(pxr::TfToken("physics:localPos0"),
              pxr::SdfValueTypeNames->Point3fArray, false).Set(
                pxr::GfVec3f(_msg.pose().position().x(),
                             _msg.pose().position().y(),
                             _msg.pose().position().z()));

      return true;
    }
    case ignition::msgs::Joint::REVOLUTE:
    {
      std::cerr << "Creating a revolute joint" << '\n';

      auto jointUSD = this->CreateRevoluteJoint("/" + this->worldName + "/" + _msg.name());

      std::cerr << "\tParent " << "/" + this->worldName + "/" + _msg.parent() << '\n';
      std::cerr << "\tchild " << "/" + this->worldName + "/" + _msg.child() << '\n';

      pxr::TfTokenVector identifiersBody0 =
          {pxr::TfToken("physics"), pxr::TfToken("body0")};

      if (pxr::UsdRelationship body0 = jointUSD.CreateRelationship(
        pxr::TfToken(pxr::SdfPath::JoinIdentifier(identifiersBody0)), false))
      {
        body0.AddTarget(pxr::SdfPath("/" + this->worldName + "/panda/" + _msg.parent()), pxr::UsdListPositionFrontOfAppendList);
      }
      else
      {
        std::cerr << "Not able to create UsdRelationship for body1" << '\n';
      }

      pxr::TfTokenVector identifiersBody1 =
          {pxr::TfToken("physics"), pxr::TfToken("body1")};

      if (pxr::UsdRelationship body1 = jointUSD.CreateRelationship(
        pxr::TfToken(pxr::SdfPath::JoinIdentifier(identifiersBody1)), false))
      {
        body1.AddTarget(pxr::SdfPath("/" + this->worldName + "/panda/" + _msg.child()), pxr::UsdListPositionFrontOfAppendList);
      }
      else
      {
        std::cerr << "Not able to create UsdRelationship for body1" << '\n';
      }

      ignition::math::Vector3i axis(
        _msg.axis1().xyz().x(),
        _msg.axis1().xyz().y(),
        _msg.axis1().xyz().z());

      if (axis == ignition::math::Vector3i(1, 0, 0))
      {
        jointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
          pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("X"));
      }
      else if (axis == ignition::math::Vector3i(0, 1, 0))
      {
        jointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
          pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("Y"));
      }
      else if (axis == ignition::math::Vector3i(0, 0, 1))
      {
        jointUSD.CreateAttribute(pxr::TfToken("physics:axis"),
          pxr::SdfValueTypeNames->Token, false).Set(pxr::TfToken("Z"));
      }

      jointUSD.CreateAttribute(pxr::TfToken("physics:localPos1"),
              pxr::SdfValueTypeNames->Point3f, false).Set(
                pxr::GfVec3f(0, 0, 0));

      jointUSD.CreateAttribute(pxr::TfToken("physics:localPos0"),
              pxr::SdfValueTypeNames->Point3f, false).Set(
                pxr::GfVec3f(
                  _msg.pose().position().x(),
                  _msg.pose().position().y(),
                  _msg.pose().position().z()));

      jointUSD.CreateAttribute(pxr::TfToken("drive:angular:physics:damping"),
              pxr::SdfValueTypeNames->Float, false).Set(100000.0f);
      jointUSD.CreateAttribute(pxr::TfToken("drive:angular:physics:stiffness"),
              pxr::SdfValueTypeNames->Float, false).Set(1000000.0f);

      jointUSD.CreateAttribute(pxr::TfToken("drive:angular:physics:targetPosition"),
              pxr::SdfValueTypeNames->Float, false).Set(0.0f);

      jointUSD.CreateAttribute(pxr::TfToken("physics:lowerLimit"),
              pxr::SdfValueTypeNames->Float, false).Set(
                static_cast<float>(_msg.axis1().limit_lower() * 180 / 3.1416));

      jointUSD.CreateAttribute(pxr::TfToken("physics:upperLimit"),
              pxr::SdfValueTypeNames->Float, false).Set(
                static_cast<float>(_msg.axis1().limit_upper() * 180 / 3.1416));

      pxr::TfToken appliedSchemaNamePhysicsArticulationRootAPI("PhysicsArticulationRootAPI");
      pxr::TfToken appliedSchemaNamePhysxArticulationAPI("PhysxArticulationAPI");
      pxr::SdfPrimSpecHandle primSpecPanda = pxr::SdfCreatePrimInLayer(
        this->stage->GetEditTarget().GetLayer(),
        pxr::SdfPath("/" + this->worldName + "/panda"));
      pxr::SdfTokenListOp listOpPanda;
      // Use ReplaceOperations to append in place.
      if (!listOpPanda.ReplaceOperations(pxr::SdfListOpTypeExplicit,
              0, 0, {appliedSchemaNamePhysicsArticulationRootAPI, appliedSchemaNamePhysxArticulationAPI})) {
        std::cerr << "Error " << '\n';
      }
      primSpecPanda->SetInfo(pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOpPanda));


      pxr::TfToken appliedSchemaName("PhysicsDriveAPI:angular");
      pxr::SdfPrimSpecHandle primSpec = pxr::SdfCreatePrimInLayer(
        this->stage->GetEditTarget().GetLayer(),
        pxr::SdfPath("/" + this->worldName + "/" + _msg.name()));
      pxr::SdfTokenListOp listOp;

      // Use ReplaceOperations to append in place.
      if (!listOp.ReplaceOperations(pxr::SdfListOpTypeExplicit,
              0, 0, {appliedSchemaName})) {
        std::cerr << "Error " << '\n';
      }

      primSpec->SetInfo(pxr::UsdTokens->apiSchemas, pxr::VtValue::Take(listOp));

      break;
    }
    default:
      return false;
  }

  return false;
}

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void SceneImpl::CallbackJoint(const ignition::msgs::Model &_msg)
{
  std::unique_lock<std::mutex> lkPose(poseMutex);

  auto it = this->models.find(_msg.name());
  if (it != this->models.end())
  {
    auto &joints = it->second.ignitionJoints;
    for (auto &joint : _msg.joint())
    {
      ignition::math::Pose3d poseJoint(
          joint.pose().position().x(), joint.pose().position().y(),
          joint.pose().position().z(), joint.pose().orientation().w(),
          joint.pose().orientation().x(), joint.pose().orientation().y(),
          joint.pose().orientation().z());

      auto itJoint = joints.find(joint.name());
      if (itJoint != joints.end())
      {
        itJoint->second->pose = poseJoint;
        itJoint->second->position = joint.axis1().position();
      }
      else
      {
        std::shared_ptr<IgnitionJoint> ignitionJoint =
            std::make_shared<IgnitionJoint>();
        ignitionJoint->pose = poseJoint;
        ignitionJoint->position = joint.axis1().position();
        joints.insert({joint.name(), ignitionJoint});
        ParseJoint(joint);
      }
    }
  }
}
}  // namespace omniverse
}  // namespace ignition
