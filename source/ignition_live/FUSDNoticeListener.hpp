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
#ifndef IGNITION_OMNIVERSE_FUSDNOTICELISTENER_HPP
#define IGNITION_OMNIVERSE_FUSDNOTICELISTENER_HPP

#include <memory>
#include <string>

#include "ThreadSafe.hpp"
#include "Scene.hpp"

#include <pxr/usd/usd/notice.h>

namespace ignition
{
namespace omniverse
{
class FUSDNoticeListener : public pxr::TfWeakBase
{
 public:
  FUSDNoticeListener(
    std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> _stage,
    const std::string &_worldName,
    Simulator _simulatorPoses);

  void Handle(const class pxr::UsdNotice::ObjectsChanged &ObjectsChanged);

  /// \internal
  /// \brief Private data pointer
  IGN_UTILS_IMPL_PTR(dataPtr)
};
}  // namespace omniverse
}  // namespace ignition

#endif
