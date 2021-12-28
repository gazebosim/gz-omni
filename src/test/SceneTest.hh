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

#include <gtest/gtest.h>

#include "../OmniverseSceneImpl.hh"

namespace ignition::rendering::omni::test {

class SceneTest : public ::testing::Test {
 protected:
  OmniverseScene::SharedPtr scene;
  OmniverseVisual::SharedPtr root_visual;

  void SetUp() override {
    this->scene = OmniverseSceneImpl::Make(0, "test_scene", nullptr);
    this->scene->Load();
    this->scene->Init();
    this->root_visual =
        std::dynamic_pointer_cast<OmniverseVisual>(this->scene->RootVisual());
  }
};

}  // namespace ignition::rendering::omni::test
