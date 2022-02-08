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
#ifndef OMNIVERSE_FUSDLAYERNOTICELISTENER_HPP
#define OMNIVERSE_FUSDLAYERNOTICELISTENER_HPP

#include "IgnitionModel.hpp"
#include "Scene.hpp"

#include "ignition/common/WorkerPool.hh"

namespace ignition
{
namespace omniverse
{
class FUSDLayerNoticeListener : public pxr::TfWeakBase
{
 public:
  FUSDLayerNoticeListener(Scene::SharedPtr& _scene,
                          const std::string& _worldName)
      : scene(_scene), worldName(_worldName)
  {
  }

  void HandleGlobalLayerReload(const pxr::SdfNotice::LayerDidReloadContent& n)
  {
    igndbg << "HandleGlobalLayerReload called" << std::endl;
  }

  // Print some interesting info about the LayerNotice
  void HandleRootOrSubLayerChange(
      const class pxr::SdfNotice::LayersDidChangeSentPerLayer& LayerNotice,
      const pxr::TfWeakPtr<pxr::SdfLayer>& Sender)
  {
    auto Iter = LayerNotice.find(Sender);
    for (auto& ChangeEntry : Iter->second.GetEntryList())
    {
      const pxr::SdfPath& sdfPath = ChangeEntry.first;

      pxr::SdfPathVector vectorPath = sdfPath.GetPrefixes();

      // for (auto &v : vectorPath)
      // {
      // 	igndbg << "v " << v.GetName() << '\n';
      // }

      if (ChangeEntry.second.flags.didRemoveNonInertPrim)
      {
        std::unordered_map<std::string, IgnitionModel> models;
        models = this->scene->GetModels();
        ignition::msgs::Entity req;

        auto it = models.find(sdfPath.GetName());
        if (it == models.end())
        {
          ignwarn << "Not able to remove [" << sdfPath.GetName() << "]"
                  << std::endl;
        }

        req.set_id(it->second.id);
        req.set_name(it->second.name);
        // req.set_name(ignition::msgs::Entity::Type::MODEL);

        ignition::msgs::Boolean rep;
        bool result;
        unsigned int timeout = 5000;
        bool executed = node.Request("/world/" + this->worldName + "/remove",
                                     req, timeout, rep, result);
        if (executed)
        {
          if (rep.data())
          {
            igndbg << "model was removed [" << sdfPath.GetName() << "]"
                   << std::endl;
            this->scene->RemoveModel(sdfPath.GetName());
          }
          else
          {
            ignerr << "Error model was not removed [" << sdfPath.GetName()
                   << "]" << std::endl;
          }
        }
        // ignmsg << " deleted " << sdfPath.GetName() << std::endl;
      }
      else if (ChangeEntry.second.flags.didAddNonInertPrim)
      {
        ignmsg << " added" << sdfPath.GetName() << std::endl;
      }

      for (auto Info : ChangeEntry.second.infoChanged)
      {
        if (Info.second.second.IsArrayValued() &&
            Info.second.second.GetArraySize() > 4)
        {
          ignmsg << " : " << Info.second.second.GetTypeName() << "["
                 << Info.second.second.GetArraySize() << "]" << std::endl;
        }
        else
        {
          std::unordered_map<std::string, IgnitionModel> models;
          models = this->scene->GetModels();

          IgnitionModel ignitionModel;
          auto it = models.find(vectorPath[1].GetName());
          if (it == models.end())
          {
            return;
          }
          ignitionModel = it->second;

          // Prepare the input parameters.
          ignition::msgs::Pose req;
          ignition::msgs::Boolean rep;

          req.set_name(vectorPath[1].GetName());
          req.set_id(ignitionModel.id);

          if (sdfPath.GetName() == "xformOp:translate")
          {
            // std::cerr << "Info.second.second.GetTypeName() " <<
            // Info.second.second.GetTypeName() << '\n';
            pxr::GfVec3f translate;
            if (Info.second.second.GetTypeName() == "GfVec3d")
            {
              pxr::GfVec3d translated = Info.second.second.Get<pxr::GfVec3d>();
              translate[0] = translated[0];
              translate[1] = translated[1];
              translate[2] = translated[2];
            }
            else if (Info.second.second.GetTypeName() == "GfVec3f")
            {
              translate = Info.second.second.Get<pxr::GfVec3f>();
            }
            req.mutable_position()->set_x(translate[0]);
            req.mutable_position()->set_y(translate[1]);
            req.mutable_position()->set_z(translate[2]);
          }
          else
          {
            req.mutable_position()->set_x(ignitionModel.pose.Pos().X());
            req.mutable_position()->set_y(ignitionModel.pose.Pos().Y());
            req.mutable_position()->set_z(ignitionModel.pose.Pos().Z());
          }

          if (sdfPath.GetName() == "xformOp:orientation")
          {
          }
          else
          {
            req.mutable_orientation()->set_x(ignitionModel.pose.Rot().X());
            req.mutable_orientation()->set_y(ignitionModel.pose.Rot().Y());
            req.mutable_orientation()->set_z(ignitionModel.pose.Rot().Z());
            req.mutable_orientation()->set_w(ignitionModel.pose.Rot().W());
          }

          ignmsg << " : " << Info.second.first << " -> " << Info.second.second
                 << std::endl;

          {
            // pool.AddWork([&] ()
            // {
            bool result;
            unsigned int timeout = 5000;
            bool executed = node.Request("/world/" + worldName + "/set_pose",
                                         req, timeout, rep, result);
            if (executed)
            {
              if (rep.data())
              {
                igndbg << "The position was setted fine" << std::endl;
              }
              else
              {
                ignerr << "The position was not setted fine" << std::endl;
              }
            }
            // });
            // bool result;
            // unsigned int timeout = 500;
            // bool executed = this->node.Request(
            // 	"/world/" + worldName + "/set_pose", req, timeout, rep, result);

            // this->scene->SetModelPose(vectorPath[1].GetName(),
            // 	ignition::math::Pose3d(
            // 		ignition::math::Vector3d(
            // 			req.mutable_position()->x(),
            // 			req.mutable_position()->y(),
            // 			req.mutable_position()->z()
            // 		),
            // 		ignition::math::Quaterniond(
            // 			req.mutable_orientation()->w(),
            // 			req.mutable_orientation()->x(),
            // 			req.mutable_orientation()->y(),
            // 			req.mutable_orientation()->z())));
            // if (executed)
            // {
            // 	std::cerr << "rep " << rep.data() << '\n';
            // }
          }
        }
      }
    }
  }
  Scene::SharedPtr scene;
  std::string worldName;
  ignition::transport::Node node;
  ignition::common::WorkerPool pool;
};
}  // namespace omniverse
}  // namespace ignition

#endif
