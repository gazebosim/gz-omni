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

#ifndef IGNITION_OMNIVERSE_CONNECT_HPP
#define IGNITION_OMNIVERSE_CONNECT_HPP

#include "OmniClientpp.hpp"

#include <pxr/usd/usd/stage.h>

#include <OmniClient.h>
#include <OmniUsdLive.h>

#include <string>

// Global for making the logging reasonable
static std::mutex gLogMutex;

namespace ignition::omniverse
{
static std::string normalizedStageUrl;

// Stage URL really only needs to contain the server in the URL.  eg.
// omniverse://ov-prod
void PrintConnectedUsername(const std::string& stageUrl);

/// \brief Creates a new ignition stage in omniverse, does nothing if the
/// stage already exists.
/// \details The new stage is authored with ignition metadata.
/// \return The url of the stage
MaybeError<std::string, GenericError> CreateOmniverseModel(
    const std::string& destinationPath);

void CheckpointFile(const char* stageUrl, const char* comment);

// Startup Omniverse
bool StartOmniverse();
}  // namespace ignition::omniverse

#endif
