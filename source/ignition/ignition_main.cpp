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

bool g_haveUpdates = false;
std::condition_variable g_cv;
std::mutex g_mutex;

template<class Mutex>
auto make_lock(Mutex& m)
{
	return std::unique_lock<Mutex>(m);
}

// #include <pxr/usd/usdPhysics/scene.h>

// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ahcorde/.local/share/ov/pkg/connectsample-103.1.0/_build/linux-x86_64/debug/
// bash build.sh
// ./_build/linux-x86_64/debug/ignition omniverse://localhost/Users/test/helloworld.usd

// Startup Omniverse
static bool startOmniverse()
{
	// Register a function to be called whenever the library wants to print something to a log
	omniClientSetLogCallback(
		[](char const* threadName, char const* component, OmniClientLogLevel level, char const* message) noexcept
		{
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
			std::cout << "Connection Status: " << omniClientGetConnectionStatusString(status) << " [" << url << "]" << std::endl;
			if (status == eOmniClientConnectionStatus_ConnectError)
			{
				// We shouldn't just exit here - we should clean up a bit, but we're going to do it anyway
				std::cout << "[ERROR] Failed connection, exiting." << std::endl;
				exit(-1);
			}
		});

	return true;
}

// The program expects one argument, a path to a USD file
int main(int argc, char* argv[])
{
  if (!startOmniverse())
  {
    std::cerr << "Not able to start Omniverse" << '\n';
    return -1;
  }

  std::string stageUrl(argv[1]);

  // // Normalize the URL because the omniUsdLiveSetModeForUrl() interface keys off of the _normalized_ stage path
	// std::string normalizedStageUrl;
	// char *normalizedStageBuffer = nullptr;
	// size_t bufferSize = 0;
	// omniClientNormalizeUrl(stageUrl.c_str(), normalizedStageUrl.data(), &bufferSize);
	// normalizedStageUrl.reserve(bufferSize);
	// normalizedStageUrl += omniClientNormalizeUrl(stageUrl.c_str(), normalizedStageUrl.data(), &bufferSize);
  //
	// std::cout << "Original Stage URL  : " << stageUrl << std::endl;
	// std::cout << "Normalized Stage URL: " << normalizedStageUrl << std::endl;
  //
	// // Enable live mode for this stage's URL
	// omniUsdLiveSetModeForUrl(normalizedStageUrl.c_str(), OmniUsdLiveMode::eOmniUsdLiveModeEnabled);

  omniUsdLiveSetDefaultEnabled(true);

	// Open the live stage
  auto stage = pxr::UsdStage::Open("salida.usd");
	if (!stage)
	{
		std::cout << "Failure to open stage.  Exiting." << std::endl;
		exit(1);
	}

  stage->SetMetadata(pxr::UsdGeomTokens->upAxis, pxr::UsdGeomTokens->z);
  stage->SetEndTimeCode(100);
  stage->SetMetadata(pxr::TfToken("metersPerUnit"), 1.0);
  stage->SetStartTimeCode(0);
  stage->SetTimeCodesPerSecond(24);

  const pxr::SdfPath worldPrimPath("/World");

  auto usdWorldPrim = pxr::UsdGeomXform::Define(stage, worldPrimPath);


  auto updateThread = std::thread([]() {
		auto lock = make_lock(g_mutex);
		while (true)
		{
			g_haveUpdates = false;
			printf("Processing live updates\n");
			omniUsdLiveProcess();
			while (!g_haveUpdates)
			{
				g_cv.wait(lock);
			}
		}
	});

  omniUsdLiveSetQueuedCallback([]() noexcept {
		{
      auto lock = make_lock(g_mutex);
			printf("Live update queued\n");
      g_haveUpdates = true;
      g_cv.notify_all();
		}
	});

  // auto usdPhysics = pxr::UsdPhysicsScene::Define(stage,
  //     pxr::SdfPath("/World/physics"));
  // const auto &sdfWorldGravity = 9.8;
  // usdPhysics.CreateGravityDirectionAttr().Set(pxr::GfVec3f(
  //       1, 1, -1));
  // usdPhysics.CreateGravityMagnitudeAttr().Set(
  //     static_cast<float>(1.0));

  auto usdCylinder = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/World/Sphere"));
  usdCylinder.CreateRadiusAttr().Set(1.0);
  usdCylinder.CreateHeightAttr().Set(2.0);
  pxr::GfVec3f endPoint(1);
  endPoint[2] = 2 * 0.5;
  pxr::VtArray<pxr::GfVec3f> extentBounds;
  extentBounds.push_back(-1.0 * endPoint);
  extentBounds.push_back(endPoint);
  usdCylinder.CreateExtentAttr().Set(extentBounds);

  usdCylinder.AddTranslateOp().Set(pxr::GfVec3d(0, 0, 1));

  stage->SetDefaultPrim(stage->GetPrimAtPath(pxr::SdfPath("/World")));

  // PXR_NS::UsdStageRefPtr g_stage;
  // g_stage = PXR_NS::UsdStage::Open(normalizedStageUrl);
  // if (!g_stage)
  // {
  //   std::cerr << "error!" << '\n';
  // }
  if (!stage->GetRootLayer()->Export("salida.usda"))
  {
    std::cerr << "Issue saving USD to salida.usd\n";
    exit(-6);
  }

  double t = 0;
  while(1)
  {
    stage->Save();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    // omniUsdLiveProcess();
    t += 0.001;
    // std::cerr << "step" << '\n';
  }

  // The stage is a sophisticated object that needs to be destroyed properly.
	// Since stage is a smart pointer we can just reset it
	stage.Reset();

  g_cv.notify_all();
	updateThread.join();

	omniClientShutdown();

  return 0;
}

