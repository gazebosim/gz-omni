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

#include <ignition/rendering.hh>
#include "OmniverseRenderEngine.hh"

namespace ignition::rendering::omni
{
  OmniverseRenderEngine *OmniverseRenderEngine::Instance()
  {
    static OmniverseRenderEngine instance;
    return &instance;
  }

  bool OmniverseRenderEngine::Load(const std::map<std::string, std::string> & /* params */)
  {
    throw std::runtime_error("Not implemented");
    std::cout << "Load" << std::endl;
    // startOmniverse(false);
    return true;
  }

  bool OmniverseRenderEngine::Init()
  {
    std::cout << "Init" << std::endl;
    return true;
  }

  void OmniverseRenderEngine::Destroy()
  {
    std::cout << "Destroy" << std::endl;
    // shutdownOmniverse();
  }

  bool OmniverseRenderEngine::Fini()
  {
    std::cout << "Fini" << std::endl;
    return true;
  }

  bool OmniverseRenderEngine::IsLoaded() const
  {
    std::cout << "IsLoaded" << std::endl;
    return true;
  }

  bool OmniverseRenderEngine::IsInitialized() const
  {
    std::cout << "IsInitialized" << std::endl;
    return true;
  }

  bool OmniverseRenderEngine::IsEnabled() const
  {
    std::cout << "IsEnabled" << std::endl;
    return true;
  }

  std::string OmniverseRenderEngine::Name() const
  {
    std::cout << "Name" << std::endl;
    return "OmniverseConnector";
  }

  unsigned int OmniverseRenderEngine::SceneCount() const
  {
    std::cout << "SceneCount" << std::endl;
    return 1;
  }

  bool OmniverseRenderEngine::HasScene(ConstScenePtr scene) const
  {
    std::cout << "HasScene" << std::endl;
    return false;
  }

  bool OmniverseRenderEngine::HasSceneId(unsigned int id) const
  {
    std::cout << "HasSceneId" << std::endl;
    return false;
  }

  bool OmniverseRenderEngine::HasSceneName(const std::string &name) const
  {
    std::cout << "HasSceneName" << std::endl;
    return false;
  }

  ScenePtr OmniverseRenderEngine::SceneById(unsigned int id) const
  {
    std::cout << "SceneById" << std::endl;
    return nullptr;
  }

  ScenePtr OmniverseRenderEngine::SceneByName(const std::string &name) const
  {
    std::cout << "SceneByName" << std::endl;
    return nullptr;
  }

  ScenePtr OmniverseRenderEngine::SceneByIndex(unsigned int index) const
  {
    std::cout << "SceneByIndex" << std::endl;
    return nullptr;
  }

  void OmniverseRenderEngine::DestroyScene(ScenePtr scene)
  {
    std::cout << "DestroyScene" << std::endl;
  }

  void OmniverseRenderEngine::DestroySceneById(unsigned int id)
  {
    std::cout << "DestroySceneById" << std::endl;
  }

  void OmniverseRenderEngine::DestroySceneByName(const std::string &name)
  {
    std::cout << "DestroySceneByName" << std::endl;
  }

  void OmniverseRenderEngine::DestroySceneByIndex(unsigned int index)
  {
    std::cout << "DestroySceneByIndex" << std::endl;
  }

  void OmniverseRenderEngine::DestroyScenes()
  {
    std::cout << "DestroyScenes" << std::endl;
  }

  ScenePtr OmniverseRenderEngine::CreateScene(const std::string &name)
  {
    std::cout << "CreateScene" << std::endl;
    return nullptr;
  }

  ScenePtr OmniverseRenderEngine::CreateScene(unsigned int id, const std::string &name)
  {
    std::cout << "CreateSceneId" << std::endl;
    return nullptr;
  }

  void OmniverseRenderEngine::SetHeadless(bool headless)
  {
    std::cout << "SetHeadless" << std::endl;
  }

  bool OmniverseRenderEngine::Headless() const
  {
    std::cout << "Headless" << std::endl;
    return false;
  }

  void OmniverseRenderEngine::AddResourcePath(const std::string &path)
  {
    std::cout << "AddResourcePath" << std::endl;
  }

  RenderPassSystemPtr OmniverseRenderEngine::RenderPassSystem() const
  {
    std::cout << "RenderPassSystem" << std::endl;
    return nullptr;
  }

  ConstScenePtr asd;
};
