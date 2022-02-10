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

#include "OmniClientpp.hpp"

#include <ignition/common/Console.hh>

#include <OmniClient.h>

namespace ignition::omniverse
{
OmniverseLock::OmniverseLock(const std::string& _url) : url(_url)
{
  omniClientLock(this->url.c_str(), nullptr, nullptr);
}

OmniverseLock::~OmniverseLock()
{
  omniClientUnlock(this->url.c_str(), nullptr, nullptr);
}

bool CheckClientResult(OmniClientResult result)
{
  return result == eOmniClientResult_Ok || result == eOmniClientResult_OkLatest;
}

OmniverseSync::MaybeError<OmniClientListEntry> OmniverseSync::Stat(
    const std::string& url) noexcept
{
  MaybeError<OmniClientListEntry> ret(eOmniClientResult_Error);
  omniClientWait(omniClientStat(
      url.c_str(), &ret,
      [](void* userData, OmniClientResult clientResult,
         const OmniClientListEntry* entry) noexcept
      {
        auto* ret =
            reinterpret_cast<MaybeError<OmniClientListEntry>*>(userData);
        if (!CheckClientResult(clientResult))
        {
          *ret = clientResult;
          return;
        }
        *ret = *entry;
      }));
  return ret;
}
}  // namespace ignition::omniverse
