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

#include "FUSDLayerNoticeListener.hpp"

#include <memory>
#include <string>

#include <ignition/transport/Node.hh>

namespace ignition
{
namespace omniverse
{
class FUSDLayerNoticeListener::Implementation
{
public:
  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> stage;
  std::string worldName;
  std::shared_ptr<ignition::transport::Node> node;
};

FUSDLayerNoticeListener::FUSDLayerNoticeListener(
  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> _stage,
  const std::string& _worldName)
    : dataPtr(ignition::utils::MakeImpl<Implementation>())
{
  this->dataPtr->stage = _stage;
  this->dataPtr->worldName = _worldName;
  this->dataPtr->node = std::make_shared<ignition::transport::Node>();
}

void FUSDLayerNoticeListener::HandleGlobalLayerReload(
  const pxr::SdfNotice::LayerDidReloadContent& n)
{
  igndbg << "HandleGlobalLayerReload called" << std::endl;
}

// Print some interesting info about the LayerNotice
void FUSDLayerNoticeListener::HandleRootOrSubLayerChange(
    const class pxr::SdfNotice::LayersDidChangeSentPerLayer& _layerNotice,
    const pxr::TfWeakPtr<pxr::SdfLayer>& _sender)
{
  auto iter = _layerNotice.find(_sender);
  for (auto & changeEntry : iter->second.GetEntryList())
  {
    const pxr::SdfPath& sdfPath = changeEntry.first;

    if (changeEntry.second.flags.didRemoveNonInertPrim)
    {
      ignition::msgs::Entity req;
      req.set_name(sdfPath.GetName());
      req.set_type(ignition::msgs::Entity::MODEL);

      ignition::msgs::Boolean rep;
      bool result;
      unsigned int timeout = 5000;
      bool executed = this->dataPtr->node->Request(
        "/world/" + this->dataPtr->worldName + "/remove",
        req, timeout, rep, result);
      if (executed)
      {
        if (rep.data())
        {
          igndbg << "Model was removed [" << sdfPath.GetName() << "]"
                 << std::endl;
          this->dataPtr->stage->Lock()->RemovePrim(sdfPath);
        }
        else
        {
          ignerr << "Error model was not removed [" << sdfPath.GetName()
                 << "]" << std::endl;
        }
      }
      ignmsg << "Deleted " << sdfPath.GetName() << std::endl;
    }
    else if (changeEntry.second.flags.didAddNonInertPrim)
    {
      ignmsg << "Added" << sdfPath.GetName() << std::endl;
    }
  }
}

}
}
