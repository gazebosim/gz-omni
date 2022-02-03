#include <chrono>         // std::chrono::seconds
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>         // std::this_thread::sleep_for

#include "OmniClient.h"
#include "OmniUsdLive.h"

#include <pxr/base/gf/vec3f.h>
#include "pxr/usd/usd/notice.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include <pxr/usd/usdGeom/cylinder.h>
#include "pxr/usd/usdGeom/metrics.h"
#include <pxr/usd/usdGeom/xform.h>

#include <sdf/Element.hh>

#include "ignition/common/Filesystem.hh"
#include "ignition/math/Vector3.hh"
#include "ignition/common/Console.hh"

// Globals for Omniverse Connection and base Stage
static pxr::UsdStageRefPtr gStage;

// Global for making the logging reasonable
static std::mutex gLogMutex;

// #include <pxr/usd/usdPhysics/scene.h>

// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ahcorde/.local/share/ov/pkg/connectsample-103.1.0/_build/linux-x86_64/debug/:/home/ahcorde/.local/share/ov/pkg/connectsample-103.1.0/_build/linux-x86_64/release/
// bash build.sh
// ./_build/linux-x86_64/debug/ignition omniverse://localhost/Users/test/helloworld.usd

// Startup Omniverse
static bool startOmniverse()
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

static void failNotify(const char *msg, const char *detail = nullptr, ...)
{
	std::unique_lock<std::mutex> lk(gLogMutex);

	fprintf(stderr, "%s\n", msg);
	if (detail != nullptr)
	{
		fprintf(stderr, "%s\n", detail);
	}
}

// Stage URL really only needs to contain the server in the URL.  eg. omniverse://ov-prod
static void printConnectedUsername(const std::string& stageUrl)
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
static std::string createOmniverseModel(const std::string& destinationPath)
{
	std::string stageUrl = destinationPath + "/ignition.usd";

	// Delete the old version of this file on Omniverse and wait for the operation to complete
	{
		std::unique_lock<std::mutex> lk(gLogMutex);
		std::cout << "Waiting for " << stageUrl << " to delete... " << std::endl;
	}
	omniClientWait(omniClientDelete(stageUrl.c_str(), nullptr, nullptr));
	{
		std::unique_lock<std::mutex> lk(gLogMutex);
		std::cout << "finished" << std::endl;
	}

	// Create this file in Omniverse cleanly
	gStage = pxr::UsdStage::CreateNew(stageUrl);
	if (!gStage)
	{
		exit(-1);
		failNotify("Failure to create model in Omniverse", stageUrl.c_str());
		return std::string();
	}

	{
		std::unique_lock<std::mutex> lk(gLogMutex);
		std::cout << "New stage created: " << stageUrl << std::endl;
	}

	// Always a good idea to declare your up-ness
	UsdGeomSetStageUpAxis(gStage, pxr::UsdGeomTokens->z);

	return stageUrl;
}

static void checkpointFile(const char* stageUrl, const char* comment)
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

// The program expects one argument, a path to a USD file
int main(int argc, char* argv[])
{
	ignition::common::Console::SetVerbosity(4);

	std::string destinationPath = "omniverse://localhost/Users/ignition";

  if (!startOmniverse())
  {
    std::cerr << "Not able to start Omniverse" << '\n';
    return -1;
  }

	// Create the USD model in Omniverse
	const std::string stageUrl = createOmniverseModel(destinationPath);

	// Print the username for the server
	printConnectedUsername(stageUrl);

  gStage->SetEndTimeCode(100);
  gStage->SetMetadata(pxr::TfToken("metersPerUnit"), 1.0);
  gStage->SetStartTimeCode(0);
  gStage->SetTimeCodesPerSecond(24);

	const pxr::SdfPath worldPrimPath("/World");

  auto usdWorldPrim = pxr::UsdGeomXform::Define(gStage, worldPrimPath);

	auto usdCylinder = pxr::UsdGeomCylinder::Define(gStage, pxr::SdfPath("/World/Cylinder"));
  usdCylinder.CreateRadiusAttr().Set(1.0);
  usdCylinder.CreateHeightAttr().Set(2.0);
  pxr::GfVec3f endPoint(1);
  endPoint[2] = 2 * 0.5;
  pxr::VtArray<pxr::GfVec3f> extentBounds;
  extentBounds.push_back(-1.0 * endPoint);
  extentBounds.push_back(endPoint);
  usdCylinder.CreateExtentAttr().Set(extentBounds);

  usdCylinder.AddTranslateOp().Set(pxr::GfVec3d(0, 0, 1));

	// Commit the changes to the USD
	gStage->Save();
	omniUsdLiveProcess();
	std::cerr << "1" << '\n';
	checkpointFile(stageUrl.c_str(), "Add box and light to stage");

  // auto usdPhysics = pxr::UsdPhysicsScene::Define(stage,
  //     pxr::SdfPath("/World/physics"));
  // const auto &sdfWorldGravity = 9.8;
  // usdPhysics.CreateGravityDirectionAttr().Set(pxr::GfVec3f(
  //       1, 1, -1));
  // usdPhysics.CreateGravityMagnitudeAttr().Set(
  //     static_cast<float>(1.0));

  // stage->SetDefaultPrim(stage->GetPrimAtPath(pxr::SdfPath("/World")));

  // if (!stage->GetRootLayer()->Export("salida.usda"))
  // {
  //   std::cerr << "Issue saving USD to salida.usd\n";
  //   exit(-6);
  // }

	// Process any updates that may have happened to the stage from another client
	omniUsdLiveWaitForPendingUpdates();
	std::cerr << "2" << '\n';

	// Define storage for the different xform ops that Omniverse Kit likes to use
	pxr::UsdGeomXformOp translateOp;
	pxr::UsdGeomXformOp rotateOp;
	pxr::UsdGeomXformOp scaleOp;
	pxr::GfVec3d position(0);
	pxr::GfVec3f rotZYX(0);
	pxr::GfVec3f scale(1);

	while (true)
	{
		// std::cerr << "3" << '\n';

		omniUsdLiveWaitForPendingUpdates();

		// Process any updates that may have happened to the stage from another client
		// omniUsdLiveWaitForPendingUpdates();
		auto cylinder = gStage->GetPrimAtPath(pxr::SdfPath("/World/Cylinder"));

		if (cylinder)
		{
			// Get the xform ops stack
			bool resetXformStack = false;
			pxr::UsdGeomXformable xForm = pxr::UsdGeomXformable(cylinder);
			std::vector<pxr::UsdGeomXformOp> xFormOps = xForm.GetOrderedXformOps(&resetXformStack);

			// Get the current xform op values
			for (size_t i = 0; i < xFormOps.size(); i++)
			{
				switch (xFormOps[i].GetOpType()) {
				case pxr::UsdGeomXformOp::TypeTranslate:
					translateOp = xFormOps[i];
					translateOp.Get(&position);
					break;
				case pxr::UsdGeomXformOp::TypeRotateZYX:
					rotateOp = xFormOps[i];
					rotateOp.Get(&rotZYX);
					break;
				case pxr::UsdGeomXformOp::TypeScale:
					scaleOp = xFormOps[i];
					scaleOp.Get(&scale);
					break;
				}
			}

			std::cerr << "position " << position << '\n';
			gStage->Save();
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
