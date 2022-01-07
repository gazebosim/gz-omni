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
#include <pxr/usd/usd/primRange.h>

#include <algorithm>
#include <fstream>

#include "../OmniverseSceneImpl.hh"
#include "test/TestConfig.hh"

namespace ignition::rendering::omni::test {

class SceneTest : public ::testing::Test {
 protected:
  OmniverseScene::SharedPtr scene;
  OmniverseVisual::SharedPtr rootVisual;

  void SetUp() override {
    auto test_info = testing::UnitTest::GetInstance()->current_test_info();
    auto stage = pxr::UsdStage::CreateNew(TestOutputDir + "/SceneTest/" +
                                          test_info->name() + ".usda");
    this->scene =
        OmniverseSceneImpl::Make(0, "test_scene", nullptr, std::move(stage));
    this->scene->Load();
    this->scene->Init();
    this->rootVisual =
        std::dynamic_pointer_cast<OmniverseVisual>(this->scene->RootVisual());
  }

  void TearDown() override { this->scene->Stage()->Save(); }

  template <typename Predicate>
  bool HasPrim(const Predicate& pred) {
    auto it = this->scene->Stage()->Traverse();
    return std::find_if(it.begin(), it.end(), [&pred](const auto& prim) {
             return pred(prim);
           }) != it.end();
  }
};

}  // namespace ignition::rendering::omni::test
