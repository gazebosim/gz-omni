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

#include <iostream>
#include <mutex>
#include <string>

#include <pxr/base/gf/vec3f.h>
#include "pxr/usd/usd/notice.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include <pxr/usd/usdGeom/cylinder.h>
#include "pxr/usd/usdGeom/metrics.h"
#include <pxr/usd/usdGeom/xform.h>

#include "OmniverseConnect.hpp"

namespace ignition
{
namespace omniverse
{
  void failNotify(const char *msg, const char *detail = nullptr, ...)
  {
  	std::unique_lock<std::mutex> lk(gLogMutex);

  	fprintf(stderr, "%s\n", msg);
  	if (detail != nullptr)
  	{
  		fprintf(stderr, "%s\n", detail);
  	}
  }

  // Stage URL really only needs to contain the server in the URL.  eg. omniverse://ov-prod
  void printConnectedUsername(const std::string& stageUrl)
  {
  	// Get the username for the connection
  	std::string userName("_none_");
  	omniClientWait(omniClientGetServerInfo(stageUrl.c_str(), &userName, [](void* userData, OmniClientResult result, struct OmniClientServerInfo const * info) noexcept
  		{
  			std::string* userName = static_cast<std::string*>(userData);
  			if (userData && userName && info && info->username)
  			{
  				userName->assign(info->username);
  			}
  		}));
  	{
  		std::unique_lock<std::mutex> lk(gLogMutex);
  		std::cout << "Connected username: " << userName << std::endl;
  	}
  }

  // Create a new connection for this model in Omniverse, returns the created stage URL
  std::string createOmniverseModel(const std::string& destinationPath, pxr::UsdStageRefPtr &_gstage)
  {
  	std::string stageUrl = destinationPath + "/shapes.usd";

  	// // Delete the old version of this file on Omniverse and wait for the operation to complete
  	// {
  	// 	std::unique_lock<std::mutex> lk(gLogMutex);
  	// 	std::cout << "Waiting for " << stageUrl << " to delete... " << std::endl;
  	// }
  	// omniClientWait(omniClientDelete(stageUrl.c_str(), nullptr, nullptr));
  	// {
  	// 	std::unique_lock<std::mutex> lk(gLogMutex);
  	// 	std::cout << "finished" << std::endl;
  	// }

  	// Create this file in Omniverse cleanly
  	// gStage = pxr::UsdStage::CreateNew(stageUrl);
  	_gstage = pxr::UsdStage::Open(stageUrl);
  	if (!_gstage)
  	{
  		failNotify("Failure to create model in Omniverse", stageUrl.c_str());
      exit(-1);
  		return std::string();
  	}

  	{
  		std::unique_lock<std::mutex> lk(gLogMutex);
  		std::cout << "New stage created: " << stageUrl << std::endl;
  	}

  	// Always a good idea to declare your up-ness
  	UsdGeomSetStageUpAxis(_gstage, pxr::UsdGeomTokens->z);

  	return stageUrl;
  }

  void checkpointFile(const char* stageUrl, const char* comment)
  {
  	bool bCheckpointsSupported = false;
  	omniClientWait(omniClientGetServerInfo(stageUrl, &bCheckpointsSupported,
  		[](void* UserData, OmniClientResult Result, OmniClientServerInfo const * Info) noexcept
  		{
  			if (Result == eOmniClientResult_Ok && Info && UserData)
  			{
  				bool* bCheckpointsSupported = static_cast<bool*>(UserData);
  				*bCheckpointsSupported = Info->checkpointsEnabled;
  			}
  		}));

  	if (bCheckpointsSupported)
  	{
  		const bool bForceCheckpoint = true;
  		omniClientWait(omniClientCreateCheckpoint(stageUrl, comment, bForceCheckpoint, nullptr,
  		[](void* userData, OmniClientResult result, char const * checkpointQuery) noexcept
  		{}));
  	}
  }

  // Startup Omniverse
  bool startOmniverse()
  {
  	// Register a function to be called whenever the library wants to print something to a log
  	omniClientSetLogCallback(
  		[](char const* threadName, char const* component, OmniClientLogLevel level, char const* message) noexcept
  		{
  			std::unique_lock<std::mutex> lk(gLogMutex);
  			std::cout << "[" << omniClientGetLogLevelString(level) << "] " << message << std::endl;
  		});

  	// The default log level is "Info", set it to "Debug" to see all messages
  	omniClientSetLogLevel(eOmniClientLogLevel_Info);

  	// Initialize the library and pass it the version constant defined in OmniClient.h
  	// This allows the library to verify it was built with a compatible version. It will
  	// return false if there is a version mismatch.
  	if (!omniClientInitialize(kOmniClientVersion))
  	{
  		return false;
  	}

  	omniClientRegisterConnectionStatusCallback(nullptr,
  		[](void* userData, const char* url, OmniClientConnectionStatus status) noexcept
  		{
  			std::unique_lock<std::mutex> lk(gLogMutex);
  			std::cout << "Connection Status: " << omniClientGetConnectionStatusString(status) << " [" << url << "]" << std::endl;
  			if (status == eOmniClientConnectionStatus_ConnectError)
  			{
  				// We shouldn't just exit here - we should clean up a bit, but we're going to do it anyway
  				std::cout << "[ERROR] Failed connection, exiting." << std::endl;
  				exit(-1);
  			}
  		});

  	// Enable live updates
  	omniUsdLiveSetDefaultEnabled(true);

  	return true;
  }
}
}
