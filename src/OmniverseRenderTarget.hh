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

#ifndef IGNITION_RENDERING_OMNI_OMNVIERSERENDERTARGET_HH
#define IGNITION_RENDERING_OMNI_OMNVIERSERENDERTARGET_HH

#include <ignition/common/Console.hh>  // FIXTHEM: missing include in BaseRenderTarget.hh
#include <ignition/rendering/base/BaseRenderTarget.hh>

#include "OmniverseObject.hh"
#include "OmniverseScene.hh"

namespace ignition::rendering::omni {

class OmniverseRenderTarget : public BaseRenderTarget<OmniverseObject> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseRenderTarget>;

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        OmniverseScene::SharedPtr _scene) {
    auto sp =
        std::shared_ptr<OmniverseRenderTarget>(new OmniverseRenderTarget());
    sp->Init(_id, _name, _scene);
    return sp;
  }

  void Copy(Image& _image) const override;

 protected:
  void RebuildImpl() override;
};

class OmniverseRenderTexture : public BaseRenderTexture<OmniverseRenderTarget> {
 public:
  using SharedPtr = std::shared_ptr<OmniverseRenderTexture>;

  static SharedPtr Make(unsigned int _id, const std::string& _name,
                        OmniverseScene::SharedPtr _scene) {
    auto sp =
        std::shared_ptr<OmniverseRenderTexture>(new OmniverseRenderTexture());
    sp->Init(_id, _name, _scene);
    return sp;
  }
};

}  // namespace ignition::rendering::omni

#endif
