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

#include <ignition/plugin/Register.hh>
#include <ignition/rendering.hh>

#include "OmniverseRenderEngine.hh"

namespace ignition::rendering::omni {
class OmniverseConnectorPlugin : public RenderEnginePlugin {
 public:
  std::string Name() const override { return "OmniverseConnector"; }

 public:
  RenderEngine *Engine() const override {
    return OmniverseRenderEngine::Instance();
  }
};

IGNITION_ADD_PLUGIN(OmniverseConnectorPlugin, RenderEnginePlugin)
}  // namespace ignition::rendering::omni
