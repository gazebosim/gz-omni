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

/**
 * C++ wrappers for various omniclient apis
 */

#include "Error.hpp"

#include <OmniClient.h>

#include <ostream>
#include <string>

namespace ignition::omniverse
{
/// \brief RAII wrapper to omniClientLock and omniClientUnlock
class OmniverseLock
{
 public:
  OmniverseLock(const std::string& _url);

  ~OmniverseLock();

  OmniverseLock(const OmniverseLock&) = delete;
  OmniverseLock(OmniverseLock&&) = delete;
  OmniverseLock& operator=(const OmniverseLock&) = delete;

 private:
  const std::string url;
};

/// \brief Synchronous API for omniverse
class OmniverseSync
{
 public:
  template <typename T>
  using MaybeError = MaybeError<T, OmniClientResult>;

  static MaybeError<OmniClientListEntry> Stat(const std::string& url) noexcept;
};
}  // namespace ignition::omniverse
