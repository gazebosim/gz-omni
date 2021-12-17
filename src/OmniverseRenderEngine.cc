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

#include "OmniverseRenderEngine.hh"

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/references.h>

#include <iostream>

#include "OmniverseSceneImpl.hh"

namespace ignition::rendering::omni {

OmniverseRenderEngine *OmniverseRenderEngine::Instance() {
  static OmniverseRenderEngine instance;
  return &instance;
}

bool OmniverseRenderEngine::LoadImpl(
    const std::map<std::string, std::string> &params) {
  // TODO: start omniverse
  return true;
}

bool OmniverseRenderEngine::InitImpl() {
  // TODO: start omniverse
  return true;
}

ScenePtr OmniverseRenderEngine::CreateSceneImpl(unsigned int _id,
                                                const std::string &_name) {
  auto scene = OmniverseSceneImpl::Make(_id, _name, this);
  this->_scenes->Add(scene);
  return scene;
}

};  // namespace ignition::rendering::omni