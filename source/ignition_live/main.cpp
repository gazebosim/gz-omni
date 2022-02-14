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

#include <ignition/common/Console.hh>
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
  stage = pxr::UsdStage::Open(stageUrl);
  ignmsg << "Opened stage [" << stageUrl << "]" << std::endl;

  omniUsdLiveSetModeForUrl(stageUrl.c_str(),
                           OmniUsdLiveMode::eOmniUsdLiveModeEnabled);

  PrintConnectedUsername(stageUrl);

  Scene::SharedPtr scene = Scene::Make(worldName, stage);

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
  double curFps = 0;

  while (true)
  {
    std::this_thread::sleep_for((lastUpdate + kUpdateRate) -
                                std::chrono::steady_clock::now());
    auto now = std::chrono::steady_clock::now();
    curFps = 1 / std::chrono::duration<double>(now - lastUpdate).count();
    lastUpdate = now;

    omniUsdLiveProcess();

    std::unordered_map<std::string, IgnitionModel> models;
    models = scene->GetModels();

    for (const auto& model : models)
    {
      auto modelUSD = scene->GetPrimAtPath("/" + worldName + "/" + model.first);
      if (modelUSD)
      {
        // Get the xform ops stack
        pxr::UsdGeomXformable xForm = pxr::UsdGeomXformable(modelUSD);

        GetOp getOp(xForm);

        {
          pxr::GfVec3d newPosition(model.second.pose.Pos().X(),
                                   model.second.pose.Pos().Y(),
                                   model.second.pose.Pos().Z());

          pxr::GfVec3f newRotZYX(model.second.pose.Rot().Roll() * 180 / 3.1416,
                                 model.second.pose.Rot().Pitch() * 180 / 3.1416,
                                 model.second.pose.Rot().Yaw() * 180 / 3.1416);

          SetOp(xForm, getOp.translateOp, pxr::UsdGeomXformOp::TypeTranslate,
                newPosition, pxr::UsdGeomXformOp::Precision::PrecisionDouble);
          SetOp(xForm, getOp.rotateOp, pxr::UsdGeomXformOp::TypeRotateXYZ,
                newRotZYX, pxr::UsdGeomXformOp::Precision::PrecisionFloat);
          // SetOp(
          // 	xForm,
          // 	scaleOp,
          // 	pxr::UsdGeomXformOp::TypeScale,
          // 	scale,
          // 	pxr::UsdGeomXformOp::Precision::PrecisionFloat);

          // Make sure the xform op order is correct (translate, rotate, scale)
          std::vector<pxr::UsdGeomXformOp> xFormOpsReordered;
          xFormOpsReordered.push_back(getOp.translateOp);
          xFormOpsReordered.push_back(getOp.rotateOp);
          // xFormOpsReordered.push_back(scaleOp);
          xForm.SetXformOpOrder(xFormOpsReordered);

          if (!model.second.ignitionJoints.empty())
          {
            for (auto& joint : model.second.ignitionJoints)
            {
              igndbg << "joint.first " << joint.first << '\n';
              auto jointUSD =
                  scene->GetPrimAtPath("/" + worldName + joint.first);
              // auto driveJoint = pxr::UsdPhysicsDriveAPI(jointUSD);
              if (!jointUSD)
              {
                igndbg << "no joint" << '\n';
              }
              else
              {
                auto attrTargetPos = jointUSD.GetAttribute(
                    pxr::TfToken("drive:angular:physics:targetPosition"));
                if (attrTargetPos)
                {
                  float pos;
                  attrTargetPos.Get(&pos);
                  attrTargetPos.Set(
                      pxr::VtValue(joint.second->position * 180.0f / 3.1416f));
                  igndbg << joint.first << " pos :" << pos << '\n';
                }
              }
            }
          }
          scene->SaveStage();
        }
      }
      else
      {
        ignerr << "Not able to find path "
               << "/" + worldName + "/" + model.first << '\n';
      }
    }
  }

  return 0;
}
