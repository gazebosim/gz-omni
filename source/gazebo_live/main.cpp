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

#include <omni/connect/core/Core.h>
#include <omni/connect/core/LayerAlgo.h>
#include <omni/connect/core/LiveSession.h>
#include <omni/connect/core/LiveSessionChannel.h>
#include <omni/connect/core/LiveSessionConfig.h>
#include <omni/connect/core/LiveSessionInfo.h>
#include <omni/connect/core/PrimAlgo.h>
#include <omni/connect/core/StageAlgo.h>
#include <omni/connect/core/XformAlgo.h>

#include <carb/extras/Path.h>
#include <carb/filesystem/IFileSystem.h>

#include <cassert>
#include <cctype>
#include <chrono>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

#include <experimental/filesystem>
#include <limits.h>
#include <unistd.h> //readlink
namespace fs = std::experimental::filesystem;

#include "GetOp.hpp"
#include "OmniverseConnect.hpp"
#include "Scene.hpp"
#include "SetOp.hpp"
#include "ThreadSafe.hpp"

#include <gz/common/Console.hh>
#include <gz/common/SystemPaths.hh>
#include <gz/common/StringUtils.hh>

#include <gz/utils/cli.hh>

#include "OmniClient.h"
#include "OmniUsdResolver.h"
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

// Initialize the Omniverse application
OMNI_APP_GLOBALS("GzOmniverseConnector", "Gazebo Omniverse Live Connector");

PXR_NAMESPACE_USING_DIRECTIVE

constexpr double kTargetFps = 60;
constexpr std::chrono::duration<double> kUpdateRate(1 / kTargetFps);

void findOrCreateSession(omni::connect::core::LiveSession* liveSession)
{
  gzmsg << "Select or create a Live Session:" << std::endl;
  UsdStageRefPtr liveStage;
  omni::connect::core::LiveSessionInfo::SessionNames sessionList = liveSession->getInfo()->getSessionNames();
  for (size_t i = 0; i < sessionList.size(); i++)
  {
    gzmsg << " [" << (int)i << "] " << sessionList[i] << std::endl;
  }
  gzmsg << " [n] Create a new session" << std::endl;
  gzmsg << " [q] Quit" << std::endl;
  gzmsg << "Select a live session to join:" << std::endl;

  char selection;
  std::cin >> selection;

  // If the user picked a session, join it by name
  size_t selectionIdx = size_t(selection) - 0x30;
  std::string sessionName;
  if (std::isdigit(selection) && selectionIdx < sessionList.size())
  {
    sessionName = sessionList[selectionIdx];
  }
  else if ('n' == selection)
  {
    // Get a new session name
    gzmsg << "Enter the new session name: " << std::endl;
    std::cin >> sessionName;
  }
  else
  {
    gzmsg << "Exiting" << std::endl;
    exit(0);
  }

  // Join the session and change the edit target to the new .live sublayer
  SdfLayerHandle liveLayer = liveSession->join(sessionName);
  if (!liveLayer)
  {
    gzerr << "Failed to join live session: " << sessionName << std::endl;
    exit(1);
  }
  else
  {
    gzmsg << "Successfully joined session: " << sessionName << std::endl;
  }
}

int main(int argc, char* argv[])
{
  CLI::App app("gz omniverse connector");

  // clang-format off
  std::string destinationPath;
  app.add_option("-p,--path", destinationPath,
                  "Location of the omniverse stage. e.g. \"omniverse://localhost/Users/gz/stage.usd\"")
      ->required();

  std::string worldName;
  app.add_option("-w,--world", worldName, "Name of the gz world")
      ->required();

  bool use_live;
  app.add_option("-l,--live", use_live,
                  "Whether live usd editing should be used")
      ->required();

  std::string scopeName;
  app.add_option("-n,--scopename", scopeName,
                  "Optional scope to be applied to the models from this world");

  std::vector<double> scopeTranslation{0, 0, 0};
  app.add_option("--scopetranslation", scopeTranslation,
                  "Optional translation to apply to the scope. Ignored if scopename is not specified")
      ->expected(3);

  double scopeRotation{0};
  app.add_option("--scoperotation", scopeRotation,
                  "Optional yaw rotation to apply to the scope. Ignored if scopename is not specified");

  gz::omniverse::Simulator simulatorPoses{
      gz::omniverse::Simulator::gz};
  std::map<std::string, gz::omniverse::Simulator> map{
      {"gz", gz::omniverse::Simulator::gz},
      {"isaacsim", gz::omniverse::Simulator::IsaacSim}};
  app.add_option("--pose", simulatorPoses,
                  "Which simulator will handle the poses")
      ->required()
      ->transform(CLI::CheckedTransformer(map, CLI::ignore_case));

  bool verbose;
  gz::common::Console::SetVerbosity(3);
  app.add_flag_callback("-v,--verbose",
                  [&verbose]() {
                    gz::common::Console::SetVerbosity(4);
                    verbose = true;
                  });
  // clang-format on

  CLI11_PARSE(app, argc, argv);

  gz::math::Pose3d scopePose;
  if (!scopeName.empty())
  {
      scopePose.Set(scopeTranslation[0], scopeTranslation[1],
          scopeTranslation[2], 0, 0, scopeRotation);
  }

  std::string gzResourcePath;

  auto systemPaths = gz::common::systemPaths();
  gz::common::env("GZ_GAZEBO_RESOURCE_PATH", gzResourcePath);
  for (const auto& resourcePath : gz::common::Split(gzResourcePath, ':'))
  {
      systemPaths->AddFilePaths(resourcePath);
  }

  // Connect with omniverse
  if (!gz::omniverse::StartOmniverse(verbose))
  {
    gzerr << "Not able to start Omniverse" << std::endl;
    return -1;
  }

  // Open the USD model in Omniverse
  const std::string stageUrl = [&]()
  {
    auto result = gz::omniverse::CreateOmniverseModel(destinationPath);
    if (!result)
    {
      gzerr << result.Error() << std::endl;
      exit(-1);
    }
    return result.Value();
  }();

  // Open the stage
  UsdStageRefPtr stage = UsdStage::Open(stageUrl);
  if (!stage)
  {
    OMNI_LOG_FATAL("Failure to open stage in Omniverse: %s", stageUrl.c_str());
    exit(1);
  }

  // If use_live is set, create a LiveSession instance - we can browse the
  // available sessions and join/create one.
  std::shared_ptr<omni::connect::core::LiveSession> liveSession;
  if (use_live)
  {
    liveSession = omni::connect::core::LiveSession::create(stage);
    if (!liveSession)
    {
      OMNI_LOG_ERROR("Failure to create a live session for stage: %s", stageUrl.c_str());
      exit(1);
    }
    findOrCreateSession(liveSession.get());

    gz::omniverse::PrintConnectedUsername(stageUrl);
  }

  gzmsg << "Initializing gz scene" << std::endl;
  gz::omniverse::Scene scene(worldName, stageUrl, stage, simulatorPoses,
                              use_live, scopeName, scopePose);
  if (!scene.Init())
  {
      return -1;
  };

  if (!use_live)
  {
    scene.Save();
    gzmsg << "Saved scene, exiting since use_live is False" << std::endl;
    return 0;
  }

  auto lastUpdate = std::chrono::steady_clock::now();
  // don't spam the console, show the fps only once a sec
  auto nextShowFps =
      lastUpdate.time_since_epoch() + std::chrono::duration<double>(1);

  scene.Save();

  while (true)
  {
    std::this_thread::sleep_for((lastUpdate + kUpdateRate) -
                                std::chrono::steady_clock::now());
    auto now = std::chrono::steady_clock::now();
    if (now.time_since_epoch() > nextShowFps)
    {
      double curFps =
          1 / std::chrono::duration<double>(now - lastUpdate).count();
      nextShowFps = now.time_since_epoch() + std::chrono::duration<double>(1);
      gzmsg << "fps: " << curFps << std::endl;
    }
    lastUpdate = now;

    std::lock_guard<std::mutex> l(scene.Mutex());
    omniClientLiveProcess();
  }

  return 0;
}
