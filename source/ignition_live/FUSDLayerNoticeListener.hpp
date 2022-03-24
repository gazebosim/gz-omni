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
#ifndef IGNITION_OMNIVERSE_FUSDLAYERNOTICELISTENER_HPP
#define IGNITION_OMNIVERSE_FUSDLAYERNOTICELISTENER_HPP

#include "Scene.hpp"

#include "ThreadSafe.hpp"

#include <pxr/usd/usd/stage.h>

#include <ignition/common/Console.hh>

#include <ignition/utils/ImplPtr.hh>

namespace ignition
{
namespace omniverse
{
class FUSDLayerNoticeListener : public pxr::TfWeakBase
{
 public:
  FUSDLayerNoticeListener(
    std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> _stage,
    const std::string& _worldName);

  void HandleGlobalLayerReload(const pxr::SdfNotice::LayerDidReloadContent& n);

  // Print some interesting info about the LayerNotice
  void HandleRootOrSubLayerChange(
      const class pxr::SdfNotice::LayersDidChangeSentPerLayer& _layerNotice,
      const pxr::TfWeakPtr<pxr::SdfLayer>& _sender);

  /// \internal
  /// \brief Private data pointer
  IGN_UTILS_IMPL_PTR(dataPtr)
};
}  // namespace omniverse
}  // namespace ignition

#endif
