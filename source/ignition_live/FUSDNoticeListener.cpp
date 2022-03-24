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
#include "FUSDNoticeListener.hpp"

#include "GetOp.hpp"

#include <ignition/common/Console.hh>

#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>

#include <ignition/transport/Node.hh>

#include <sdf/Collision.hh>
#include <sdf/Geometry.hh>
#include <sdf/Root.hh>
#include <sdf/Link.hh>
#include <sdf/Model.hh>
#include <sdf/Sphere.hh>
#include <sdf/Visual.hh>

namespace ignition
{
namespace omniverse
{

class FUSDNoticeListener::Implementation
{
public:

  void ParseCube(const pxr::UsdPrim &_prim, sdf::Link &_link);
  void ParseCylinder(const pxr::UsdPrim &_prim, sdf::Link &_link);
  void ParseSphere(const pxr::UsdPrim &_prim, sdf::Link &_link);

  bool ParsePrim(const pxr::UsdPrim &_prim, sdf::Link &_link)
  {
    std::cerr << "ParsePrim " << '\n';
    if (_prim.IsA<pxr::UsdGeomSphere>())
    {
      std::cerr << "ParseSphere " << '\n';

      ParseSphere(_prim, _link);
      return true;
    }
    else if (_prim.IsA<pxr::UsdGeomCylinder>())
    {
      ParseCylinder(_prim, _link);
    }
    return false;
  }

  void CreateSDF(sdf::Link &_link, const pxr::UsdPrim &_prim)
  {
    std::cerr << "CreateSDF " << '\n';
    if (!_prim)
      return;
    if (ParsePrim(_prim, _link))
    {
      return;
    }
    else
    {
      auto children = _prim.GetChildren();
      for (const pxr::UsdPrim &childPrim : children)
      {
        if (ParsePrim(childPrim, _link))
        {
          return;
        }
        else
        {
          CreateSDF(_link, childPrim);
        }
      }
    }
  }

  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> stage;
  std::string worldName;
  std::unordered_map<std::string, transport::Node::Publisher> revoluteJointPublisher;

  /// \brief Ignition communication node.
  public: std::shared_ptr<transport::Node> node;

  Simulator simulatorPoses;
};

void FUSDNoticeListener::Implementation::ParseCube(
  const pxr::UsdPrim &_prim, sdf::Link &_link)
{
  // double size;
  // auto variant_cylinder = pxr::UsdGeomCube(_prim);
  // variant_cylinder.GetSizeAttr().Get(&size);
}

void FUSDNoticeListener::Implementation::ParseCylinder(
  const pxr::UsdPrim &_prim, sdf::Link &_link)
{
  // auto variant_cylinder = pxr::UsdGeomCylinder(_prim);
  // double radius;
  // double height;
  // variant_cylinder.GetRadiusAttr().Get(&radius);
  // variant_cylinder.GetHeightAttr().Get(&height);
}

void FUSDNoticeListener::Implementation::ParseSphere(
  const pxr::UsdPrim &_prim, sdf::Link &_link)
{
  double radius;
  auto variant_sphere = pxr::UsdGeomSphere(_prim);
  variant_sphere.GetRadiusAttr().Get(&radius);

  // By default issac sim uses millimeters
  radius *= 0.01;

  sdf::Visual visual;
  sdf::Collision collision;
  sdf::Geometry geom;
  sdf::Sphere sphere;

  geom.SetType(sdf::GeometryType::SPHERE);
  sphere.SetRadius(radius);
  geom.SetSphereShape(sphere);
  visual.SetName("sphere_visual");
  visual.SetGeom(geom);

  collision.SetName("sphere_collision");
  collision.SetGeom(geom);

  _link.AddVisual(visual);
  _link.AddCollision(collision);
}

FUSDNoticeListener::FUSDNoticeListener(
  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> _stage,
  const std::string &_worldName,
  Simulator _simulatorPoses)
    : dataPtr(ignition::utils::MakeImpl<Implementation>())
{
  this->dataPtr->stage = _stage;
  this->dataPtr->worldName = _worldName;
  this->dataPtr->simulatorPoses = _simulatorPoses;
  this->dataPtr->node = std::make_shared<ignition::transport::Node>();
}

void FUSDNoticeListener::Handle(
  const class pxr::UsdNotice::ObjectsChanged &ObjectsChanged)
{
  for (const pxr::SdfPath &objectsChanged : ObjectsChanged.GetResyncedPaths())
  {
    ignmsg << "Resynced Path: " << objectsChanged.GetText() << std::endl;
    auto stage = this->dataPtr->stage->Lock();
    auto modelUSD = stage->GetPrimAtPath(objectsChanged);
    std::string primName = modelUSD.GetName();

    if (primName.find("ROS_") != std::string::npos ||
        primName.find("PhysicsScene") != std::string::npos)
    {
      continue;
    }

    if (modelUSD)
    {
      // std::string strPath = objectsChanged.GetText();
      // if (strPath.find("_link") != std::string::npos
      //    || strPath.find("_visual") != std::string::npos
      //    || strPath.find("geometry") != std::string::npos) {
      //   return;
      // }

      sdf::Root root;

      sdf::Model model;

      model.SetName(modelUSD.GetPath().GetName());

      model.SetRawPose(ignition::math::Pose3d());

      sdf::Link link;
      link.SetName(modelUSD.GetPath().GetName());

    	this->dataPtr->CreateSDF(link, modelUSD);

      model.AddLink(link);
      root.SetModel(model);

    	// Prepare the input parameters.
    	ignition::msgs::EntityFactory req;
    	req.set_sdf(root.ToElement()->ToString(""));
    	req.set_name(modelUSD.GetPath().GetName());
    	req.set_allow_renaming(false);

      std::cerr << "root.ToElement()->ToString("") " << root.ToElement()->ToString("") << '\n';

      ignition::msgs::Boolean rep;
      bool result;
      unsigned int timeout = 5000;
      bool executed = this->dataPtr->node->Request(
        "/world/" + this->dataPtr->worldName + "/create",
        req, timeout, rep, result);
    	if (executed)
      {
    		if (rep.data())
    		{
    			std::cerr << "Model was inserted [" << modelUSD.GetPath().GetName()
                    << "]" << '\n';
    		}
    		else
    		{
    			std::cerr << "Error model was not inserted" << '\n';
    		}
    	}
    }
  }

  ignition::msgs::Pose_V req;

  if (this->dataPtr->simulatorPoses == Simulator::IssacSim)
  {
    // this loop checks all paths to find revolute joints
    // if there is some, we get the body0 and body1 and calculate the
    // joint angle.
    auto stage = this->dataPtr->stage->Lock();
    auto range = pxr::UsdPrimRange::Stage(*stage);
    // int count = 0;
    for (auto const &prim : range)
    {
      std::string primType = prim.GetPrimTypeInfo().GetTypeName().GetText();
      if (primType == std::string("PhysicsRevoluteJoint"))
      {
        auto attrTargetPos = prim.GetAttribute(
            pxr::TfToken("drive:angular:physics:targetPosition"));

        auto relBody0 = prim.GetRelationships();
        std::vector<ignition::math::Quaterniond> qVector;
        for (auto & rel : relBody0)
        {
          pxr::SdfPathVector paths;
          rel.GetTargets(&paths);
          for (auto p: paths)
          {
            auto modelUSD = stage->GetPrimAtPath(p);
            auto xform = pxr::UsdGeomXformable(modelUSD);
            auto transforms = GetOp(xform);
            ignition::math::Quaterniond qOrient(
              transforms.rotQ.GetReal(),
              transforms.rotQ.GetImaginary()[0],
              transforms.rotQ.GetImaginary()[1],
              transforms.rotQ.GetImaginary()[2]);
            qVector.emplace_back(qOrient);
            std::cerr << "pxr::TfStringify(p) " << pxr::TfStringify(p)  << "\t" << qOrient.Yaw()*180/3.1416 << '\n';
          }
        }

        double angle_L1L2;
        ignition::math::Vector3d axis_L1L2;
        auto R_L1L2 = (qVector[0].Inverse() * qVector[1]);
        std::cerr << "q " << R_L1L2.W() << "\t"
                          << R_L1L2.X() << "\t"
                          << R_L1L2.Y() << "\t"
                          << R_L1L2.Z() << '\n';
        R_L1L2.ToAxis(axis_L1L2, angle_L1L2);
        axis_L1L2.Normalize();
        // std::cerr << "angle " << angle_L1L2*180/3.1416 << '\n';
        // std::cerr << "axis " << axis_L1L2.X() << "\t"
        //                      << axis_L1L2.Y() << "\t"
        //                      << axis_L1L2.Z() << '\n';
        double pos = axis_L1L2.Dot(ignition::math::Vector3d::UnitZ) * angle_L1L2;
        // std::cerr << "pos : " << pos*180/3.1416 << '\n';


        if (attrTargetPos)
        {
          // Subscribe to commands
          std::string topic = transport::TopicUtils::AsValidTopic(
            std::string("/model/") + std::string("panda") +
            std::string("/joint/") + prim.GetPath().GetName() +
            std::string("/0/cmd_pos"));

          auto pub = this->dataPtr->revoluteJointPublisher.find(topic);
          if (pub == this->dataPtr->revoluteJointPublisher.end())
          {
            this->dataPtr->revoluteJointPublisher[topic] =
              this->dataPtr->node->Advertise<msgs::Double>(topic);
          }
          else
          {
            msgs::Double cmd;
            cmd.set_data(pos);
            pub->second.Publish(cmd);
          }
        }
        else
        {
          prim.CreateAttribute(
            pxr::TfToken("drive:angular:physics:targetPosition"),
            pxr::SdfValueTypeNames->Float, false).Set(0.0f);
        }
      }
    }

    for (const pxr::SdfPath &objectsChanged :
        ObjectsChanged.GetChangedInfoOnlyPaths())
    {
      if (std::string(objectsChanged.GetText()) == "/")
        continue;
      auto stage = this->dataPtr->stage->Lock();
      igndbg << "path " << objectsChanged.GetText() << std::endl;
      auto modelUSD = stage->GetPrimAtPath(objectsChanged.GetParentPath());
      auto property = modelUSD.GetPropertyAtPath(objectsChanged);
      std::string strProperty = property.GetBaseName().GetText();
      if (strProperty == "radius")
      {
        double radius;
        auto attribute = modelUSD.GetAttributeAtPath(objectsChanged);
        attribute.Get(&radius);
      }
      if (strProperty == "translate")
      {
        auto xform = pxr::UsdGeomXformable(modelUSD);

        auto transforms = GetOp(xform);
        auto currentPrim = modelUSD;
        ignition::math::Quaterniond q(
          transforms.rotXYZ[0],
          transforms.rotXYZ[1],
          transforms.rotXYZ[2]);
        if (currentPrim.GetName() == "geometry")
        {
          currentPrim = currentPrim.GetParent();
          auto visualXform = pxr::UsdGeomXformable(currentPrim);
          auto visualOp = GetOp(visualXform);
          transforms.position += visualOp.position;
          ignition::math::Quaterniond qX, qY, qZ;
          ignition::math::Angle angleX(IGN_DTOR(visualOp.rotXYZ[0]));
          ignition::math::Angle angleY(IGN_DTOR(visualOp.rotXYZ[1]));
          ignition::math::Angle angleZ(IGN_DTOR(visualOp.rotXYZ[2]));
          qX = ignition::math::Quaterniond(angleX.Normalized().Radian(), 0, 0);
          qY = ignition::math::Quaterniond(0, angleY.Normalized().Radian(), 0);
          qZ = ignition::math::Quaterniond(0, 0, angleZ.Normalized().Radian());
          q = ((q * qX) * qY) * qZ;
          transforms.scale = pxr::GfVec3f(
            transforms.scale[0] * visualOp.scale[0],
            transforms.scale[1] * visualOp.scale[1],
            transforms.scale[2] * visualOp.scale[2]);
        }
        auto currentPrimName = currentPrim.GetName().GetString();
        int substrIndex = currentPrimName.size() - std::string("_visual").size();
        if (substrIndex >= 0 && substrIndex < currentPrimName.size())
        {
          if (currentPrimName.substr(substrIndex).find("_visual") !=
            std::string::npos)
          {
            currentPrim = currentPrim.GetParent();
            auto linkXform = pxr::UsdGeomXformable(currentPrim);
            auto linkOp = GetOp(linkXform);
            transforms.position += linkOp.position;
            ignition::math::Quaterniond qX, qY, qZ;
            ignition::math::Angle angleX(IGN_DTOR(linkOp.rotXYZ[0]));
            ignition::math::Angle angleY(IGN_DTOR(linkOp.rotXYZ[1]));
            ignition::math::Angle angleZ(IGN_DTOR(linkOp.rotXYZ[2]));
            qX = ignition::math::Quaterniond(angleX.Normalized().Radian(), 0, 0);
            qY = ignition::math::Quaterniond(0, angleY.Normalized().Radian(), 0);
            qZ = ignition::math::Quaterniond(0, 0, angleZ.Normalized().Radian());
            q = ((q * qX) * qY) * qZ;
            transforms.scale = pxr::GfVec3f(
              transforms.scale[0] * linkOp.scale[0],
              transforms.scale[1] * linkOp.scale[1],
              transforms.scale[2] * linkOp.scale[2]);
          }
        }
        currentPrimName = currentPrim.GetName().GetString();
        substrIndex = currentPrimName.size() - std::string("_link").size();
        if (substrIndex >= 0 && substrIndex < currentPrimName.size())
        {
          if (currentPrimName.substr(substrIndex).find("_link") !=
              std::string::npos)
          {
            currentPrim = currentPrim.GetParent();
            auto modelXform = pxr::UsdGeomXformable(currentPrim);
            auto modelOp = GetOp(modelXform);
            transforms.position += modelOp.position;
            ignition::math::Quaterniond qX, qY, qZ;
            ignition::math::Angle angleX(IGN_DTOR(modelOp.rotXYZ[0]));
            ignition::math::Angle angleY(IGN_DTOR(modelOp.rotXYZ[1]));
            ignition::math::Angle angleZ(IGN_DTOR(modelOp.rotXYZ[2]));
            qX = ignition::math::Quaterniond(angleX.Normalized().Radian(), 0, 0);
            qY = ignition::math::Quaterniond(0, angleY.Normalized().Radian(), 0);
            qZ = ignition::math::Quaterniond(0, 0, angleZ.Normalized().Radian());
            q = ((q * qX) * qY) * qZ;
            transforms.scale = pxr::GfVec3f(
              transforms.scale[0] * modelOp.scale[0],
              transforms.scale[1] * modelOp.scale[1],
              transforms.scale[2] * modelOp.scale[2]);
          }
        }

        std::size_t found = std::string(currentPrim.GetName()).find("_link");
        if (found != std::string::npos)
          continue;
        found = std::string(currentPrim.GetName()).find("_visual");
        if (found != std::string::npos)
          continue;

        auto poseMsg = req.add_pose();
        poseMsg->set_name(currentPrim.GetName());

        poseMsg->mutable_position()->set_x(transforms.position[0]);
        poseMsg->mutable_position()->set_y(transforms.position[1]);
        poseMsg->mutable_position()->set_z(transforms.position[2]);

        poseMsg->mutable_orientation()->set_x(q.X());
        poseMsg->mutable_orientation()->set_y(q.Y());
        poseMsg->mutable_orientation()->set_z(q.Z());
        poseMsg->mutable_orientation()->set_w(q.W());
      }
    }
    if (req.pose_size() > 0)
    {
      bool result;
      ignition::msgs::Boolean rep;
      unsigned int timeout = 500;
      bool executed = this->dataPtr->node->Request(
        "/world/" + this->dataPtr->worldName + "/set_pose_vector",
        req, timeout, rep, result);
      if (executed)
      {
        if (!result)
          ignerr << "Service call failed" << std::endl;
      }
      else
        ignerr << "Service [/world/" << this->dataPtr->worldName
               << "/set_pose_vector] call timed out" << std::endl;
    }
  }
}
}  // namespace omniverse
}  // namespace ignition
