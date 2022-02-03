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

#ifndef OMNIVERSE_CONNECT_HPP
#define OMNIVERSE_CONNECT_HPP

#include <string>

#include "pxr/usd/usd/stage.h"

#include "OmniClient.h"
#include "OmniUsdLive.h"

// Global for making the logging reasonable
static std::mutex gLogMutex;

namespace ignition
{
namespace omniverse
{
  // Stage URL really only needs to contain the server in the URL.  eg. omniverse://ov-prod
  void printConnectedUsername(const std::string& stageUrl);

  // Create a new connection for this model in Omniverse, returns the created stage URL
  std::string createOmniverseModel(const std::string& destinationPath, pxr::UsdStageRefPtr &_gstage);

  void checkpointFile(const char* stageUrl, const char* comment);

  // Startup Omniverse
  bool startOmniverse();
}
}

#endif
