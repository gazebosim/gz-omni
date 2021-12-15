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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEOBJECT_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEOBJECT_HH

#include <ignition/rendering/base/BaseObject.hh>

namespace ignition::rendering::omni {

class OmniverseObject : public BaseObject {
 public:
  explicit OmniverseObject(ScenePtr _scene) : _scene(_scene) {}

  ScenePtr Scene() const override { return _scene; }

 protected:
  OmniverseObject() = default;

 private:
  ScenePtr _scene;
};

}  // namespace ignition::rendering::omni

#endif
