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

#include <ignition/rendering.hh>

namespace ignition::rendering::omni
{
  class OmniverseRenderEngine : public RenderEngine
  {
  public:
    static OmniverseRenderEngine *Instance();

    bool Load(const std::map<std::string, std::string> & /* params */) override;
    bool Init() override;
    void Destroy() override;
    bool Fini() override;
    bool IsLoaded() const override;
    bool IsInitialized() const override;
    bool IsEnabled() const override;
    std::string Name() const override;
    unsigned int SceneCount() const override;
    bool HasScene(rendering::ConstScenePtr scene) const override;
    bool HasSceneId(unsigned int id) const override;
    bool HasSceneName(const std::string &name) const override;
    ScenePtr SceneById(unsigned int id) const override;
    ScenePtr SceneByName(const std::string &name) const override;
    ScenePtr SceneByIndex(unsigned int index) const override;
    void DestroyScene(ScenePtr scene) override;
    void DestroySceneById(unsigned int id) override;
    void DestroySceneByName(const std::string &name) override;
    void DestroySceneByIndex(unsigned int index) override;
    void DestroyScenes() override;
    ScenePtr CreateScene(const std::string &name) override;
    ScenePtr CreateScene(unsigned int id, const std::string &name) override;
    void SetHeadless(bool headless) override;
    bool Headless() const override;
    void AddResourcePath(const std::string &path) override;
    RenderPassSystemPtr RenderPassSystem() const override;
  };
}

#endif
