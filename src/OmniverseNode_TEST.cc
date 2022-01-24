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

#include "OmniverseNode.hh"

#include <gtest/gtest.h>

#include "test/SceneFixture.hh"

namespace ignition::rendering::omni::test {

struct SuiteName {
  inline static const std::string Value = "NodeTest";
};
using NodeTest = SceneFixture<SuiteName>;

TEST_F(NodeTest, GetAndSetLocalPose) {
  const auto node = OmniverseNode::Make(0, "test_node", this->scene);
  const math::Quaterniond rot(IGN_PI, IGN_PI_2, IGN_PI_4);
  node->SetLocalPose(
      math::Pose3d(math::Vector3d(10, 20, 30), rot));
  const auto pose = node->LocalPose();
  EXPECT_DOUBLE_EQ(pose.X(), 10);
  EXPECT_DOUBLE_EQ(pose.Y(), 20);
  EXPECT_DOUBLE_EQ(pose.Z(), 30);
  EXPECT_DOUBLE_EQ(pose.Roll(), rot.Roll());
  EXPECT_DOUBLE_EQ(pose.Pitch(), rot.Pitch());
  EXPECT_DOUBLE_EQ(pose.Yaw(), rot.Yaw());
}

TEST_F(NodeTest, GetAndSetLocalScale) {
  const auto node = OmniverseNode::Make(0, "test_node", this->scene);
  const math::Quaterniond rot(IGN_PI, IGN_PI_2, IGN_PI_4);
  node->SetLocalScale(math::Vector3d(1, 2, 3));
  const auto scale = node->LocalScale();
  EXPECT_DOUBLE_EQ(scale.X(), 1);
  EXPECT_DOUBLE_EQ(scale.Y(), 2);
  EXPECT_DOUBLE_EQ(scale.Z(), 3);
}

}  // namespace ignition::rendering::omni::test
