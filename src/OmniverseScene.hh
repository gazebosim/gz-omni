/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSESCENE_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSESCENE_HH

#include <pxr/usd/usd/stage.h>

#include <ignition/rendering/base/BaseScene.hh>
#include <ignition/rendering/base/BaseStorage.hh>

namespace ignition::rendering::omni {

class OmniverseScene : public BaseScene {
 public:
  using SharedPtr = std::shared_ptr<OmniverseScene>;

  virtual pxr::UsdStageRefPtr Stage() const = 0;

 protected:
  OmniverseScene(unsigned int _id, const std::string &_name)
      : BaseScene(_id, _name) {}
};

using OmniverseSceneStore = BaseSceneStore<OmniverseScene>;

}  // namespace ignition::rendering::omni

#endif
