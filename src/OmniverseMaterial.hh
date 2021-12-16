/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS  * WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 */

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEMATERIAL_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEMATERIAL_HH

#include <ignition/rendering/base/BaseMaterial.hh>
#include <ignition/rendering/base/BaseStorage.hh>

#include "OmniverseObject.hh"

namespace ignition::rendering::omni {

class OmniverseMaterial : public BaseMaterial<OmniverseObject> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseMaterial>;

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        ScenePtr _scene) {
    auto sp = std::shared_ptr<OmniverseMaterial>(new OmniverseMaterial());
    sp->InitObject(_id, _name, _scene);
    return sp;
  }
};

using OmniverseMaterialMap = BaseMaterialMap<OmniverseMaterial>;

}  // namespace ignition::rendering::omni

#endif
