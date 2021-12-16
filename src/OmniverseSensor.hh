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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSESENSOR_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSESENSOR_HH

#include <ignition/rendering/base/BaseSensor.hh>
#include <ignition/rendering/base/BaseStorage.hh>

#include "OmniverseNode.hh"

namespace ignition::rendering::omni {

class OmniverseSensor : public BaseSensor<OmniverseNode> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseSensor>;

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        ScenePtr _scene) {
    auto sp = std::shared_ptr<OmniverseSensor>(new OmniverseSensor());
    sp->InitObject(_id, _name, _scene);
    return sp;
  }
};

using OmniverseSensorStore = BaseSensorStore<OmniverseSensor>;

}  // namespace ignition::rendering::omni

#endif
