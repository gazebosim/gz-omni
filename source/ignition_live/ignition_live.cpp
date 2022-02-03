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

#include <string>

#include <pxr/usd/usd/stage.h>
#include <ignition/common/Console.hh>

#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include "FUSDLayerNoticeListener.hpp"
#include "FUSDNoticeListener.hpp"
#include "GetOp.hpp"
#include "OmniverseConnect.hpp"
#include "SceneImpl.hpp"
#include "SetOp.hpp"

// The program expects one argument, a path to a USD file
int main(int argc, char* argv[])
{
	pxr::UsdStageRefPtr gStage;
	ignition::common::Console::SetVerbosity(4);

	std::string worldName = "shapes";

	std::string filename = argv[1];

	// TODO(ahcorde): This should be an input argument
	std::string destinationPath = "omniverse://localhost/Users/ignition/" + filename;

	// Connect with omniverse
  if (!ignition::omniverse::startOmniverse())
  {
    std::cerr << "Not able to start Omniverse" << '\n';
    return -1;
  }

	// Open the USD model in Omniverse
	// TODO(ahcorde): For now, we can modify a preload a world in IssacSim but we
	// cannot create the object
	const std::string stageUrl = ignition::omniverse::createOmniverseModel(
		destinationPath, gStage);

	ignition::omniverse::printConnectedUsername(stageUrl);

	ignition::omniverse::SceneImpl::SharedPtr scene =
		ignition::omniverse::SceneImpl::Make(worldName, gStage);

	ignition::omniverse::FUSDLayerNoticeListener USDLayerNoticeListener(
		scene, worldName);
	auto LayerReloadKey = pxr::TfNotice::Register(
		pxr::TfCreateWeakPtr(&USDLayerNoticeListener),
		&ignition::omniverse::FUSDLayerNoticeListener::HandleGlobalLayerReload);
	auto LayerChangeKey = pxr::TfNotice::Register(
		pxr::TfCreateWeakPtr(&USDLayerNoticeListener),
		&ignition::omniverse::FUSDLayerNoticeListener::HandleRootOrSubLayerChange,
		gStage->GetRootLayer());

	ignition::omniverse::FUSDNoticeListener USDNoticeListener(scene, worldName);
	auto USDNoticeKey = pxr::TfNotice::Register(
		pxr::TfCreateWeakPtr(&USDNoticeListener),
		&ignition::omniverse::FUSDNoticeListener::Handle);

	// Process any updates that may have happened to the stage from another client
	omniUsdLiveWaitForPendingUpdates();

	while (true)
	{
		omniUsdLiveWaitForPendingUpdates();

		std::unordered_map<std::string, ignition::omniverse::IgnitionModel> models;
		models = scene->GetModels();

		for (const auto &model: models)
		{
			auto modelUSD = scene->GetPrimAtPath(
				"/" + worldName + "/" + model.first);
			if (modelUSD)
			{
				// Get the xform ops stack
				pxr::UsdGeomXformable xForm = pxr::UsdGeomXformable(modelUSD);

				ignition::omniverse::GetOp getOp(xForm);

				{
					pxr::GfVec3d newPosition(
						model.second.pose.Pos().X(),
					  model.second.pose.Pos().Y(),
						model.second.pose.Pos().Z());

					pxr::GfVec3f newRotZYX(
						model.second.pose.Rot().Roll() * 180 / 3.1416,
						model.second.pose.Rot().Pitch() * 180 / 3.1416,
						model.second.pose.Rot().Yaw() * 180 / 3.1416);

					ignition::omniverse::SetOp(
						xForm,
						getOp.translateOp,
						pxr::UsdGeomXformOp::TypeTranslate,
						newPosition,
						pxr::UsdGeomXformOp::Precision::PrecisionDouble);
					ignition::omniverse::SetOp(
						xForm,
						getOp.rotateOp,
						pxr::UsdGeomXformOp::TypeRotateXYZ,
						newRotZYX,
						pxr::UsdGeomXformOp::Precision::PrecisionFloat);
					// ignition::omniverse::SetOp(
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
						for(auto & joint : model.second.ignitionJoints)
						{
							std::cerr << "joint.first " << joint.first << '\n';
							auto jointUSD = scene->GetPrimAtPath(
								"/" + worldName + joint.first);
							// auto driveJoint = pxr::UsdPhysicsDriveAPI(jointUSD);
							if (!jointUSD)
							{
								std::cerr << "no joint" << '\n';
							}
							else
							{
								auto attrTargetPos = jointUSD.GetAttribute(
									pxr::TfToken("drive:angular:physics:targetPosition"));
								if (attrTargetPos)
								{
									float pos;
									attrTargetPos.Get(&pos);
									attrTargetPos.Set(pxr::VtValue(joint.second->position * 180.0f / 3.1416f));
									std::cerr << joint.first << " pos :" << pos << '\n';
								}
							}
						}
					}
					scene->SaveStage();
				}
			}
			else
			{
				std::cerr << "Not able to find path " << "/" + worldName + "/" + model.first << '\n';
			}
		}
	}

  return 0;
}
