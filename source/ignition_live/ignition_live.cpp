#include <algorithm>
#include <chrono>         // std::chrono::seconds
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>         // std::this_thread::sleep_for

#include "OmniverseConnect.hpp"
#include "SetOp.hpp"

#include <pxr/base/gf/vec3f.h>
#include "pxr/usd/usd/notice.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include <pxr/usd/usdGeom/cylinder.h>
#include "pxr/usd/usdGeom/metrics.h"
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include <sdf/Element.hh>

#include <ignition/common/Filesystem.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/common/Console.hh>

#include <ignition/transport.hh>

#include <ignition/msgs/pose.pb.h>

#include <pxr/usd/usdShade/connectableAPI.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

using namespace std::chrono_literals;

std::unordered_map<std::string, ignition::math::Pose3d> modelPoses;

// bash build.sh
// bash run_ignition_live.sh

std::mutex poseMutex;

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void cb(const ignition::msgs::Pose_V &_msg)
{
	// std::cout << "Msg: ";

	std::unique_lock<std::mutex> lkPose(poseMutex);

	for (int i = 0; i < _msg.pose().size(); ++i)
	{
		const auto &poseMsg = _msg.pose(i);
		auto it = modelPoses.find(poseMsg.name());
		if (it != modelPoses.end())
		{
			it->second = ignition::math::Pose3d(poseMsg.position().x(),
		                         poseMsg.position().y(),
		                         poseMsg.position().z(),
		                         poseMsg.orientation().w(),
		                         poseMsg.orientation().x(),
		                         poseMsg.orientation().y(),
		                         poseMsg.orientation().z());
		  it->second.Correct();
			// {
			// 	// std::unique_lock<std::mutex> lk(gLogMutex);
				// std::cout << poseMsg.name() << " : " << poseMsg.position().x() << "\t";
			// }
		}
	}
	// std::cout << std::endl;
}

auto printvector = [](const auto& v) {
    std::cout << "{ ";
    for (auto i : v) std::cout << i << ' ';
    std::cout << "} " << "\n";
};


  /// \brief Fill Material shader attributes and properties
  /// \param[in] _prim USD primitive
  /// \param[in] _name Name of the field attribute or property
  /// \param[in] _vType Type of the field
  /// \param[in] _value Value of the field
  /// \param[in] _customData Custom data to set the field
  /// \param[in] _displayName Display name
  /// \param[in] _displayGroup Display group
  /// \param[in] _doc Documentation of the field
  /// \param[in] _colorSpace if the material is a texture, we can specify the
  /// colorSpace of the image
  template<typename T>
  void CreateMaterialInput(
    const pxr::UsdPrim &_prim,
    const std::string &_name,
    const pxr::SdfValueTypeName &_vType,
    T _value,
    const std::map<pxr::TfToken, pxr::VtValue> &_customData,
    const pxr::TfToken &_displayName = pxr::TfToken(""),
    const pxr::TfToken &_displayGroup = pxr::TfToken(""),
    const std::string &_doc = "",
    const pxr::TfToken &_colorSpace = pxr::TfToken(""))
  {
    auto shader = pxr::UsdShadeShader(_prim);
    if (shader)
    {
      auto existingInput = shader.GetInput(pxr::TfToken(_name));
      pxr::SdfValueTypeName vTypeName;
      if (_vType.IsScalar())
      {
        vTypeName = _vType.GetScalarType();
      }
      else if (_vType.IsArray())
      {
        vTypeName = _vType.GetArrayType();
      }
      auto surfaceInput = shader.CreateInput(
        pxr::TfToken(_name), vTypeName);
      surfaceInput.Set(_value);
      auto attr = surfaceInput.GetAttr();

      for (const auto &[key, customValue] : _customData)
      {
        attr.SetCustomDataByKey(key, customValue);
      }
      if (!_displayName.GetString().empty())
      {
        attr.SetDisplayName(_displayName);
      }
      if (!_displayGroup.GetString().empty())
      {
        attr.SetDisplayGroup(_displayGroup);
      }
      if (!_doc.empty())
      {
        attr.SetDocumentation(_doc);
      }
      if (!_colorSpace.GetString().empty())
      {
        attr.SetColorSpace(_colorSpace);
      }
    }
    else
    {
      std::cerr << "Not able to convert the prim to a UsdShadeShader\n";
    }
  }

// The program expects one argument, a path to a USD file
int main(int argc, char* argv[])
{
	pxr::UsdStageRefPtr gStage;
	ignition::common::Console::SetVerbosity(4);

	// subscribe to:
	// - /world/<world name>/pose/info : This topic will give us
	//      the data of all object
	//
	ignition::transport::Node node;

	std::string topic = "/world/shapes/pose/info";
	// Subscribe to a topic by registering a callback.
  if (!node.Subscribe(topic, cb))
  {
    std::cerr << "Error subscribing to topic [" << topic << "]" << std::endl;
    return -1;
  }
	// topic = "/world/shapes/dynamic_pose/info";
	// // Subscribe to a topic by registering a callback.
  // if (!node.Subscribe(topic, cb))
  // {
  //   std::cerr << "Error subscribing to topic [" << topic << "]" << std::endl;
  //   return -1;
  // }
	// Request scene info to get all the models in the scene
	// keep them all in the data structure

	bool addedPrim = false;

	std::thread t([&]()
	{
		while(true)
		{
			std::set<std::string> modelNames;

			for (auto &model : modelPoses)
			{
				modelNames.insert(model.first);
			}

			// Prepare the input parameters.
		  ignition::msgs::Empty req;
		  ignition::msgs::Scene rep;
		  bool result;
		  unsigned int timeout = 5000;
		  bool executed = node.Request("/world/shapes/scene/info", req, timeout, rep, result);
			if (executed)
		  {
				std::set<std::string> modelNamesReceived;

				for (auto &model : rep.model())
				{
					modelNamesReceived.insert(model.name());
				}

				std::vector<std::string> removed;
		    std::vector<std::string> added;

				std::set_difference(modelNamesReceived.begin(), modelNamesReceived.end(),
														modelNames.begin(), modelNames.end(),
		                        std::inserter(added, added.begin()));

		    std::set_difference(modelNames.begin(), modelNames.end(),
														modelNamesReceived.begin(), modelNamesReceived.end(),
		                        std::inserter(removed, removed.begin()));

				{
					std::unique_lock<std::mutex> lkPose(poseMutex);

					if (added.size() > 0 )
					{
						std::cerr << "added " << '\n';
						printvector(added);
					}

					if (removed.size() > 0 && modelNames.size() > 0)
					{
						std::cerr << "removed " << '\n';
						printvector(removed);
						for (auto & removeModelName : removed)
						{
							gStage->RemovePrim(pxr::SdfPath("/shapes/" + removeModelName));
							modelPoses.erase(removeModelName);
						}
					}

					for (auto &model : rep.model())
					{
						auto it = modelPoses.find(model.name());
						if (it == modelPoses.end() && modelNames.size() > 0)
						{
							std::string sdfModelPath = std::string("/shapes/") + model.name();
							auto usdModelXform = pxr::UsdGeomXform::Define(gStage, pxr::SdfPath(sdfModelPath));
							for (auto & link : model.link())
							{
								std::string sdfLinkPath = sdfModelPath + "/" + link.name();
								auto usdLinkXform = pxr::UsdGeomXform::Define(gStage, pxr::SdfPath(sdfLinkPath));
								for (auto & visual : link.visual())
								{
									std::string sdfVisualPath = sdfLinkPath + "/" + visual.name();
									auto usdVisualXform = pxr::UsdGeomXform::Define(gStage, pxr::SdfPath(sdfVisualPath));
									auto geom = visual.geometry();
									std::string sdfGeometryPath = sdfVisualPath + "/geometry";

									const std::string mtl_path = "/Looks/Material_" + visual.name();
									pxr::UsdShadeMaterial material;
									material = pxr::UsdShadeMaterial::Define(gStage, pxr::SdfPath(mtl_path));
									auto usdShader = pxr::UsdShadeShader::Define(
							      gStage,
							      pxr::SdfPath(mtl_path + "/Shader"));

									auto shaderPrim = gStage->GetPrimAtPath(pxr::SdfPath(mtl_path + "/Shader"));
							    if (!shaderPrim)
							    {
							      std::cerr << "Not able to cast the UsdShadeShader to a Prim" << '\n';
							    }
									else
									{
										auto shader_out = pxr::UsdShadeConnectableAPI(shaderPrim).CreateOutput(
								      pxr::TfToken("out"), pxr::SdfValueTypeNames->Token);
								    material.CreateSurfaceOutput(
								      pxr::TfToken("mdl")).ConnectToSource(shader_out);
								    material.CreateVolumeOutput(
								      pxr::TfToken("mdl")).ConnectToSource(shader_out);
								    material.CreateDisplacementOutput(
								      pxr::TfToken("mdl")).ConnectToSource(shader_out);
								    usdShader.GetImplementationSourceAttr().Set(
								      pxr::UsdShadeTokens->sourceAsset);
								    usdShader.SetSourceAsset(
								      pxr::SdfAssetPath("OmniPBR.mdl"), pxr::TfToken("mdl"));
								    usdShader.SetSourceAssetSubIdentifier(
								      pxr::TfToken("OmniPBR"), pxr::TfToken("mdl"));

										std::map<pxr::TfToken, pxr::VtValue> customDataDiffuse =
									    {
									      {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(0.2, 0.2, 0.2))},
									      {pxr::TfToken("range:max"),
									       pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
									      {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}
									    };
								    ignition::math::Color diffuse(
											visual.material().diffuse().r(),
											visual.material().diffuse().g(),
											visual.material().diffuse().b(),
											visual.material().diffuse().a());
								    CreateMaterialInput<pxr::GfVec3f>(
								      shaderPrim,
								      "diffuse_color_constant",
								      pxr::SdfValueTypeNames->Color3f,
								      pxr::GfVec3f(diffuse.R(), diffuse.G(), diffuse.B()),
								      customDataDiffuse,
								      pxr::TfToken("Base Color"),
								      pxr::TfToken("Albedo"),
								      "This is the base color");

								    std::map<pxr::TfToken, pxr::VtValue> customDataEmissive =
								    {
								      {pxr::TfToken("default"), pxr::VtValue(pxr::GfVec3f(1, 0.1, 0.1))},
								      {pxr::TfToken("range:max"),
								       pxr::VtValue(pxr::GfVec3f(100000, 100000, 100000))},
								      {pxr::TfToken("range:min"), pxr::VtValue(pxr::GfVec3f(0, 0, 0))}
								    };
								    ignition::math::Color emissive(
											visual.material().emissive().r(),
											visual.material().emissive().g(),
											visual.material().emissive().b(),
											visual.material().emissive().a());
								    CreateMaterialInput<pxr::GfVec3f>(
								      shaderPrim,
								      "emissive_color",
								      pxr::SdfValueTypeNames->Color3f,
								      pxr::GfVec3f(emissive.R(), emissive.G(), emissive.B()),
								      customDataEmissive,
								      pxr::TfToken("Emissive Color"),
								      pxr::TfToken("Emissive"),
								      "The emission color");

								    std::map<pxr::TfToken, pxr::VtValue> customDataEnableEmission =
								    {
								      {pxr::TfToken("default"), pxr::VtValue(0)}
								    };

								    CreateMaterialInput<bool>(
								      shaderPrim,
								      "enable_emission",
								      pxr::SdfValueTypeNames->Bool,
								      emissive.A() > 0,
								      customDataEnableEmission,
								      pxr::TfToken("Enable Emissive"),
								      pxr::TfToken("Emissive"),
								      "Enables the emission of light from the material");

								    std::map<pxr::TfToken, pxr::VtValue> customDataIntensity =
								    {
								      {pxr::TfToken("default"), pxr::VtValue(40)},
								      {pxr::TfToken("range:max"), pxr::VtValue(100000)},
								      {pxr::TfToken("range:min"), pxr::VtValue(0)}
								    };
								    CreateMaterialInput<float>(
								      shaderPrim,
								      "emissive_intensity",
								      pxr::SdfValueTypeNames->Float,
								      emissive.A(),
								      customDataIntensity,
								      pxr::TfToken("Emissive Intensity"),
								      pxr::TfToken("Emissive"),
								      "Intensity of the emission");
									}

									if (geom.type() == ignition::msgs::Geometry::SPHERE)
									{
										auto usdSphere = pxr::UsdGeomSphere::Define(gStage, pxr::SdfPath(sdfGeometryPath));
										double radius = geom.sphere().radius();
										usdSphere.CreateRadiusAttr().Set(radius);
										pxr::VtArray<pxr::GfVec3f> extentBounds;
										extentBounds.push_back(pxr::GfVec3f(-1.0 * radius));
										extentBounds.push_back(pxr::GfVec3f(radius));
										usdSphere.CreateExtentAttr().Set(extentBounds);
										addedPrim = true;
										pxr::UsdShadeMaterialBindingAPI(usdSphere).Bind(material);
									}
									else if (geom.type() == ignition::msgs::Geometry::CYLINDER)
									{
										auto usdCylinder = pxr::UsdGeomCylinder::Define(gStage, pxr::SdfPath(sdfGeometryPath));
										double radius = geom.cylinder().radius();
										double length = geom.cylinder().length();

								    usdCylinder.CreateRadiusAttr().Set(radius);
								    usdCylinder.CreateHeightAttr().Set(length);
								    pxr::GfVec3f endPoint(radius);
								    endPoint[2] = length * 0.5;
								    pxr::VtArray<pxr::GfVec3f> extentBounds;
								    extentBounds.push_back(-1.0 * endPoint);
								    extentBounds.push_back(endPoint);
								    usdCylinder.CreateExtentAttr().Set(extentBounds);
										pxr::UsdShadeMaterialBindingAPI(usdCylinder).Bind(material);
									}
									else if (geom.type() == ignition::msgs::Geometry::BOX)
									{
										auto usdCube = pxr::UsdGeomCube::Define(gStage, pxr::SdfPath(sdfGeometryPath));
								    usdCube.CreateSizeAttr().Set(1.0);
								    pxr::GfVec3f endPoint(0.5);
								    pxr::VtArray<pxr::GfVec3f> extentBounds;
								    extentBounds.push_back(-1.0 * endPoint);
								    extentBounds.push_back(endPoint);
								    usdCube.CreateExtentAttr().Set(extentBounds);

								    pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
								    cubeXformAPI.SetScale(pxr::GfVec3f(
								          geom.box().size().x(),
								          geom.box().size().y(),
								          geom.box().size().z()));
										pxr::UsdShadeMaterialBindingAPI(usdCube).Bind(material);
									}
									else if (geom.type() == ignition::msgs::Geometry::PLANE)
									{
										auto usdCube = pxr::UsdGeomCube::Define(gStage, pxr::SdfPath(sdfGeometryPath));
										usdCube.CreateSizeAttr().Set(1.0);
										pxr::GfVec3f endPoint(0.5);
										pxr::VtArray<pxr::GfVec3f> extentBounds;
										extentBounds.push_back(-1.0 * endPoint);
										extentBounds.push_back(endPoint);
										usdCube.CreateExtentAttr().Set(extentBounds);

										pxr::UsdGeomXformCommonAPI cubeXformAPI(usdCube);
										cubeXformAPI.SetScale(pxr::GfVec3f(
													geom.plane().size().x(),
													geom.plane().size().y(),
													0.25));
										pxr::UsdShadeMaterialBindingAPI(usdCube).Bind(material);
									}
									else if (geom.type() == ignition::msgs::Geometry::CAPSULE)
									{
										auto usdCapsule = pxr::UsdGeomCapsule::Define(gStage, pxr::SdfPath(sdfGeometryPath));
										double radius = geom.capsule().radius();
										double length = geom.capsule().length();
										usdCapsule.CreateRadiusAttr().Set(radius);
										usdCapsule.CreateHeightAttr().Set(length);
										pxr::GfVec3f endPoint(radius);
										endPoint[2] += 0.5 * length;
										pxr::VtArray<pxr::GfVec3f> extentBounds;
										extentBounds.push_back(-1.0 * endPoint);
										extentBounds.push_back(endPoint);
										usdCapsule.CreateExtentAttr().Set(extentBounds);
										pxr::UsdShadeMaterialBindingAPI(usdCapsule).Bind(material);
									}
									else if (geom.type() == ignition::msgs::Geometry::ELLIPSOID)
									{
										auto usdEllipsoid = pxr::UsdGeomSphere::Define(gStage, pxr::SdfPath(sdfGeometryPath));
										const auto maxRadii = ignition::math::Vector3d(
											geom.ellipsoid().radii().x(),
										  geom.ellipsoid().radii().y(),
										  geom.ellipsoid().radii().z()).Max();
										usdEllipsoid.CreateRadiusAttr().Set(maxRadii);
										pxr::UsdGeomXformCommonAPI xform(usdEllipsoid);
										xform.SetScale(pxr::GfVec3f{
											geom.ellipsoid().radii().x() / maxRadii,
											geom.ellipsoid().radii().y() / maxRadii,
											geom.ellipsoid().radii().z() / maxRadii,
										});
										// extents is the bounds before any transformation
										pxr::VtArray<pxr::GfVec3f> extentBounds;
										extentBounds.push_back(pxr::GfVec3f{-maxRadii});
										extentBounds.push_back(pxr::GfVec3f{maxRadii});
										usdEllipsoid.CreateExtentAttr().Set(extentBounds);
										pxr::UsdShadeMaterialBindingAPI(usdEllipsoid).Bind(material);
									}
								}
							}
						}

						modelPoses.insert({model.name(),
							ignition::math::Pose3d(model.pose().position().x(),
						                         model.pose().position().y(),
						                         model.pose().position().z(),
						                         model.pose().orientation().w(),
						                         model.pose().orientation().x(),
						                         model.pose().orientation().y(),
						                         model.pose().orientation().z())});
					}
				}
			}
			else
			{
				std::cerr << "Not able to get the models from ignition" << '\n';
			}
			std::this_thread::sleep_for(50ms);
		}
	});



	// TODO(ahcorde): This should be an input argument
	std::string destinationPath = "omniverse://localhost/Users/ignition";

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

	// Print the username for the server
	ignition::omniverse::printConnectedUsername(stageUrl);

	// Process any updates that may have happened to the stage from another client
	omniUsdLiveWaitForPendingUpdates();

	while (true)
	{
		omniUsdLiveWaitForPendingUpdates();

		std::unordered_map<std::string, ignition::math::Pose3d> modelPosesTmp;
		{
			std::unique_lock<std::mutex> lkPose(poseMutex);
			modelPosesTmp = modelPoses;
		}

		// Process any updates that may have happened to the stage from another client
		for (const auto &model: modelPosesTmp)
		{
			auto modelUSD = gStage->GetPrimAtPath(pxr::SdfPath("/shapes/" + model.first));

			if (modelUSD)
			{
				// Get the xform ops stack
				bool resetXformStack = false;
				pxr::UsdGeomXformable xForm = pxr::UsdGeomXformable(modelUSD);
				std::vector<pxr::UsdGeomXformOp> xFormOps = xForm.GetOrderedXformOps(&resetXformStack);

				// Define storage for the different xform ops that Omniverse Kit likes to use
				pxr::UsdGeomXformOp translateOp;
				pxr::UsdGeomXformOp rotateOp;
				pxr::UsdGeomXformOp scaleOp;
				pxr::GfVec3d position(0);
				pxr::GfVec3f rotZYX(0);
				pxr::GfVec3f scale(1);

				// Get the current xform op values
				for (size_t i = 0; i < xFormOps.size(); i++)
				{
					switch (xFormOps[i].GetOpType()) {
					case pxr::UsdGeomXformOp::TypeTranslate:
						translateOp = xFormOps[i];
						translateOp.Get(&position);
						break;
					case pxr::UsdGeomXformOp::TypeRotateXYZ:
						rotateOp = xFormOps[i];
						rotateOp.Get(&rotZYX);
						break;
					case pxr::UsdGeomXformOp::TypeScale:
						scaleOp = xFormOps[i];
						scaleOp.Get(&scale);
						break;
					}
				}

				{
					pxr::GfVec3d newPosition(
						model.second.Pos().X(),
					  model.second.Pos().Y(),
						model.second.Pos().Z());

					pxr::GfVec3f newRotZYX(
						model.second.Rot().Roll() * 180 / 3.1416,
						model.second.Rot().Pitch() * 180 / 3.1416,
						model.second.Rot().Yaw() * 180 / 3.1416);

					ignition::omniverse::SetOp(
						xForm,
						translateOp,
						pxr::UsdGeomXformOp::TypeTranslate,
						newPosition,
						pxr::UsdGeomXformOp::Precision::PrecisionDouble);
					ignition::omniverse::SetOp(
						xForm,
						rotateOp,
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
					xFormOpsReordered.push_back(translateOp);
					xFormOpsReordered.push_back(rotateOp);
					// xFormOpsReordered.push_back(scaleOp);
					xForm.SetXformOpOrder(xFormOpsReordered);
				}

				// {
					// std::unique_lock<std::mutex> lk(gLogMutex);
					// std::cerr << model.first << " position " << position << '\n';
				// 	// std::cerr << model.first << " rotZYX " << rotZYX << '\n';
				// 	// std::cerr << model.first << " newRotZYX " << newRotZYX << '\n';
				// 	// std::cerr << "poseBox " << poseBox << '\n';
				// }
				gStage->Save();
				if (addedPrim)
				{
					addedPrim = false;
					omniUsdLiveProcess();
				}

				// std::this_thread::sleep_for(20ms);
			}
		}
	}

	// Shut down Omniverse connection
	// Calling this prior to shutdown ensures that all pending live updates complete.
	omniUsdLiveWaitForPendingUpdates();
	// The stage is a sophisticated object that needs to be destroyed properly.
  // Since gStage is a smart pointer we can just reset it
	gStage.Reset();
	omniClientShutdown();

  return 0;
}
