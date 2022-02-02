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
#ifndef OMNIVERSE_IGNITION_VISUAL_HPP
#define OMNIVERSE_IGNITION_VISUAL_HPP

#include <memory>
#include <string>

#include <ignition/math/Color.hh>

#include <ignition/msgs/visual.pb.h>

#include "IgnitionGeometry.hpp"
#include "Scene.hpp"

namespace ignition
{
namespace omniverse
{
class IgnitionVisual
{
public:
 using SharedPtr = std::shared_ptr<IgnitionVisual>;

 static SharedPtr Make(
     unsigned int _id,
     const std::string& _name,
     Scene::SharedPtr &_scene);

 std::vector<IgnitionGeometry::SharedPtr> Geometries() const;

 bool AttachGeometry(
   const ignition::msgs::Visual &_visual,
   const std::string &_path);

 bool DetachGeometry(
   const ignition::msgs::Visual &_visual,
   const std::string &_path);

private:
 std::vector<IgnitionGeometry::SharedPtr> geomStore;
 uint id;
 std::string name;
 Scene::SharedPtr scene;
};
}
}

#endif
