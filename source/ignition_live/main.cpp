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

#include "FUSDLayerNoticeListener.hpp"
#include "FUSDNoticeListener.hpp"
#include "GetOp.hpp"
#include "OmniverseConnect.hpp"
#include "Scene.hpp"
#include "SetOp.hpp"
#include "ThreadSafe.hpp"

#include <ignition/common/Console.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/common/StringUtils.hh>

#include <ignition/utils/cli.hh>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include <string>

using namespace ignition::omniverse;

constexpr double kTargetFps = 60;
constexpr std::chrono::duration<double> kUpdateRate(1 / kTargetFps);

int main(int argc, char* argv[])
{
  CLI::App app("Ignition omniverse connector");

  std::string destinationPath;
  app.add_option("-p,--path", destinationPath,
                 // clang-format off
                 "Location of the omniverse stage. e.g. \"omniverse://localhost/Users/ignition/stage.usd\"")
      // clang-format on
      ->required();
  std::string worldName;
  app.add_option("-w,--world", worldName, "Name of the ignition world")
      ->required();
  app.add_flag_callback("-v,--verbose",
                        []() { ignition::common::Console::SetVerbosity(4); });

  CLI11_PARSE(app, argc, argv);

  std::string ignGazeboResourcePath;
  auto systemPaths = ignition::common::systemPaths();
  ignition::common::env("IGN_GAZEBO_RESOURCE_PATH", ignGazeboResourcePath);
  for (const auto& resourcePath :
       ignition::common::Split(ignGazeboResourcePath, ':'))
  {
    systemPaths->AddFilePaths(resourcePath);
  }

  pxr::UsdStageRefPtr stage;

  // Connect with omniverse
  if (!StartOmniverse())
  {
    ignerr << "Not able to start Omniverse" << std::endl;
    return -1;
  }

  // Open the USD model in Omniverse
  // TODO(ahcorde): For now, we can modify a preload a world in IssacSim but we
  // cannot create the object
  const std::string stageUrl = [&]()
  {
    auto result = CreateOmniverseModel(destinationPath);
    if (!result)
    {
      ignerr << result.Error() << std::endl;
      exit(-1);
    }
    return result.Value();
  }();

  omniUsdLiveSetModeForUrl(stageUrl.c_str(),
                           OmniUsdLiveMode::eOmniUsdLiveModeEnabled);

  PrintConnectedUsername(stageUrl);

  Scene scene(worldName, stageUrl);
  if (!scene.Init())
  {
    return -1;
  };

  // TODO: disabled omniverse -> ignition sync to focus on ignition -> omniverse
  // FUSDLayerNoticeListener USDLayerNoticeListener(scene, worldName);
  // auto LayerReloadKey = pxr::TfNotice::Register(
  //     pxr::TfCreateWeakPtr(&USDLayerNoticeListener),
  //     &FUSDLayerNoticeListener::HandleGlobalLayerReload);
  // auto LayerChangeKey = pxr::TfNotice::Register(
  //     pxr::TfCreateWeakPtr(&USDLayerNoticeListener),
  //     &FUSDLayerNoticeListener::HandleRootOrSubLayerChange,
  //     stage->GetRootLayer());

  FUSDNoticeListener USDNoticeListener(scene, worldName);
  auto USDNoticeKey = pxr::TfNotice::Register(
      pxr::TfCreateWeakPtr(&USDNoticeListener), &FUSDNoticeListener::Handle);

  auto lastUpdate = std::chrono::steady_clock::now();
  // don't spam the console, show the fps only once a sec
  auto nextShowFps =
      lastUpdate.time_since_epoch() + std::chrono::duration<double>(1);

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
      igndbg << "fps: " << curFps << std::endl;
    }
    lastUpdate = now;

    scene.Save();
    omniUsdLiveProcess();
  }

  return 0;
}
