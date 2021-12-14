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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSERENDERENGINE_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSERENDERENGINE_HH

#include <pxr/usd/usd/stage.h>

#include <ignition/rendering/base/BaseRenderEngine.hh>

#include "OmniverseStorage.hh"

namespace ignition::rendering::omni
{
  class OmniverseRenderEngine : public BaseRenderEngine
  {
  public:
    static OmniverseRenderEngine *Instance();

    inline std::string Name() const override { return "OmniverseConnector"; }

  protected:
    bool LoadImpl(const std::map<std::string, std::string> &_params) override;
    bool InitImpl() override;
    ScenePtr CreateSceneImpl(unsigned int _id, const std::string &_name) override;

    inline SceneStorePtr Scenes() const override { return this->_scenes; }

  private:
    pxr::UsdStageRefPtr _stage;
    std::shared_ptr<OmniverseSceneStore> _scenes;
  };
}

#endif
