/*###############################################################################
#
# Copyright 2021 NVIDIA Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###############################################################################*/

/*###############################################################################
#
# The Omniverse USDA Watcher is a command line program that keeps an updated 
# USDA file on local disk that maps to a USD resident on a Nucleus server.
#	* Two arguments, the USD stage URL to watch and the output USDA stage URL
#		* Acceptable forms:
#			* omniverse://localhost/Users/test/helloworld.usd
#			* C:\USD\helloworld.usda
#			* A relative path based on the CWD of the program (helloworld.usda)
#	* Initialize Omniverse
#		* Set the Omniverse Client log callback (using a lambda)
#		* Set the Omniverse Client log level
#		* Initialize the Omniverse Client library
#		* Register a connection status callback (using a lambda)
#	* Normalize the USD stage URL
#	* Set the USD stage URL as live
#	* Open the USD stage
#	* Create and register the layer reload, layer change, and USD notice listeners
#	* Subscribe to file changes with omniClientStatSubscribe
#	* Start a thread that loops, receiving live changes from other clients
#		* When the stage is modified it's written out to the specified USDA
#	* The main thread loops on keyboard input, waiting for a 'q' or ESC
#	* Cleanup the callbacks (unsubscribe and revoke)
#	* Destroy the stage object
#	* Shutdown the Omniverse Client library
#
# eg. omniUsdaWatcher.exe omniverse://localhost/Users/test/helloworld.usd C:\USD\helloworld.usda
#
###############################################################################*/

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include "OmniClient.h"
#include "OmniUsdLive.h"
#include "pxr/usd/usd/notice.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/metrics.h"
#ifdef _WIN32
#include <conio.h>
#endif


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
	omniClientSetLogLevel(eOmniClientLogLevel_Warning);

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

class FUSDLayerNoticeListener : public pxr::TfWeakBase
{
public:
	FUSDLayerNoticeListener(){}

	void HandleGlobalLayerReload(const pxr::SdfNotice::LayerDidReloadContent& n)
	{
		std::cout << "HandleGlobalLayerReload called" << std::endl;
	}
	
	// Print some interesting info about the LayerNotice
	void HandleRootOrSubLayerChange(const class pxr::SdfNotice::LayersDidChangeSentPerLayer& LayerNotice, const pxr::TfWeakPtr<pxr::SdfLayer>& Sender)
	{
		auto Iter = LayerNotice.find(Sender);
		for (auto& ChangeEntry : Iter->second.GetEntryList())
		{
			std::cout << "ChangeEntry: " << ChangeEntry.first.GetText();
			if (ChangeEntry.second.flags.didRemoveNonInertPrim)
			{
				std::cout << " deleted" << std::endl;
			}
			else if (ChangeEntry.second.flags.didAddNonInertPrim)
			{
				std::cout << " added" << std::endl;
			}

			for (auto Info : ChangeEntry.second.infoChanged)
			{
				if (Info.second.second.IsArrayValued() && Info.second.second.GetArraySize() > 4)
				{
					std::cout << " : " << Info.second.second.GetTypeName() << "[" << Info.second.second.GetArraySize() << "]" << std::endl;
				}
				else
				{
					std::cout << " : " << Info.second.first;
					std::cout << " -> " << Info.second.second << std::endl;
				}
			}
		}

	}
};

class FUSDNoticeListener : public pxr::TfWeakBase
{
public:
	FUSDNoticeListener(){}
	void Handle(const class pxr::UsdNotice::ObjectsChanged& ObjectsChanged)
	{
		for (const pxr::SdfPath& Path : ObjectsChanged.GetResyncedPaths())
		{
			std::cout << "Resynced Path: " << Path.GetText() << std::endl;
		}
		for (const pxr::SdfPath& Path : ObjectsChanged.GetChangedInfoOnlyPaths())
		{
			std::cout << "Changed Info Path: " << Path.GetText() << std::endl;
		}

	}
};


// This struct is context for the omniClientStatSubscribe() callbacks
struct StatSubscribeContext
{
	std::string* stageUrlPtr;
	std::string* usdaPathPtr;
	pxr::UsdStageRefPtr stage;
	std::time_t* lastUpdatedTimePtr;
};

// Called immediately due to the stat subscribe function
static void clientStatCallback(void* userData, OmniClientResult result, struct OmniClientListEntry const* entry) noexcept
{
	StatSubscribeContext* context = static_cast<StatSubscribeContext*>(userData);

	if (result != OmniClientResult::eOmniClientResult_Ok)
	{
		std::cout << "Error: stage not found: " << *context->stageUrlPtr << std::endl;
		exit(1);
	}
}

// Called due to the stat subscribe function when the file is updated
static void clientStatSubscribeCallback(void* userData, OmniClientResult result, OmniClientListEvent listEvent, struct OmniClientListEntry const* entry) noexcept
{
	StatSubscribeContext* context = static_cast<StatSubscribeContext*>(userData);

	switch (listEvent)
	{
	case eOmniClientListEvent_Updated:
	{
		std::cout << "Updated - user: " << entry->modifiedBy << " version: " << entry->version << std::endl;

		// Mark the last updated time
		*context->lastUpdatedTimePtr = std::time(0);
		break;
	}
	case eOmniClientListEvent_Created:
		std::cout << "Created: " << entry->createdBy << std::endl;
		break;
	case eOmniClientListEvent_Deleted:
		std::cout << "Deleted: " << entry->createdBy << std::endl;
		exit(1);
		break;
	case eOmniClientListEvent_Locked:
		std::cout << "Locked: " << entry->createdBy << std::endl;
		break;
	default:
		break;

	}
}

// This class contains a doWork method that's use as a thread's function
//	and members that allow for synchronization between the the file update
//  callbacks and a main thread that takes keyboard input
// It busy-loops using `omniUsdLiveProcess` to recv updates from the server
//  to the USD stage.  If there are updates it will export the USDA file
//  within a second of receiving them.
class UsdaStageWriterWorker 
{
public:
	UsdaStageWriterWorker() : stopped(false) {};
	void doWork() {
		std::time_t currentTime = std::time(0);
		while (!stopped)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
			omniUsdLiveProcess();
			currentTime = std::time(0);
			// export USDA if it's been more than a second and the last update was after the last USDA export time
			if (currentTime - *lastUpdateTime > 0 && *lastUsdaWriteTime <= *lastUpdateTime)
			{
				std::cout << "Writing USDA file...";
				if (!stage->GetRootLayer()->Export(*usdaPath))
				{
					std::cout << "Unable to export stage" << std::endl;
				}
				std::cout << " complete." << std::endl;
				*lastUsdaWriteTime = std::time(0);
			}
		}
	}
	std::atomic<bool> stopped;
	pxr::UsdStageRefPtr stage;
	std::time_t* lastUpdateTime;
	std::time_t* lastUsdaWriteTime;
	std::string* usdaPath;
};

// The program expects two arguments, input and output paths to a USD file
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Please provide an Omniverse stage URL to read and a local file path to write the USDA file." << std::endl;
        return -1;
    }

    std::cout << "Omniverse USDA Watcher: " << argv[1] << " -> " << argv[2] << std::endl;
	
	std::string stageUrl(argv[1]);
	std::string usdaPath(argv[2]);

	startOmniverse();

	// Normalize the URL because the omniUsdLiveSetModeForUrl() interface keys off of the _normalized_ stage path
	std::string normalizedStageUrl;
	char *normalizedStageBuffer = nullptr;
	size_t bufferSize = 0;
	omniClientNormalizeUrl(stageUrl.c_str(), normalizedStageUrl.data(), &bufferSize);
	normalizedStageUrl.reserve(bufferSize);
	normalizedStageUrl += omniClientNormalizeUrl(stageUrl.c_str(), normalizedStageUrl.data(), &bufferSize);

	std::cout << "Original Stage URL  : " << stageUrl << std::endl;
	std::cout << "Normalized Stage URL: " << normalizedStageUrl << std::endl;

	// Enable live mode for this stage's URL
	omniUsdLiveSetModeForUrl(normalizedStageUrl.c_str(), OmniUsdLiveMode::eOmniUsdLiveModeEnabled);

	// Open the live stage
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(normalizedStageUrl);
	if (!stage)
	{
		std::cout << "Failure to open stage.  Exiting." << std::endl;
		exit(1);
	}

	// Create and register the layer reload, layer change, and USD notice listeners
	FUSDLayerNoticeListener USDLayerNoticeListener;
	auto LayerReloadKey = pxr::TfNotice::Register(pxr::TfCreateWeakPtr(&USDLayerNoticeListener), &FUSDLayerNoticeListener::HandleGlobalLayerReload);
	auto LayerChangeKey = pxr::TfNotice::Register(pxr::TfCreateWeakPtr(&USDLayerNoticeListener), &FUSDLayerNoticeListener::HandleRootOrSubLayerChange, stage->GetRootLayer());

	FUSDNoticeListener USDNoticeListener;
	auto USDNoticeKey = pxr::TfNotice::Register(pxr::TfCreateWeakPtr(&USDNoticeListener), &FUSDNoticeListener::Handle);

	// Some time variables to know how long it's been since we've exported the USDA file
	std::time_t lastUpdateTime = std::time(0);
	std::time_t lastUsdaWriteTime = std::time(0);
	
	// Initialize "user data" for the stat subscribe callbacks
	StatSubscribeContext userData;
	userData.stageUrlPtr = &normalizedStageUrl;
	userData.usdaPathPtr = &usdaPath;
	userData.stage = stage;
	userData.lastUpdatedTimePtr = &lastUpdateTime;

	// Subscribe to stat callbacks for the live stage that we're watching
	// This isn't absolutely necessary since we have the USD Notices, but
	//  this would work well for texture or material reload
	OmniClientRequestId statSubscribeRequestId = omniClientStatSubscribe(
		normalizedStageUrl.c_str(),
		&userData,
		clientStatCallback,
		clientStatSubscribeCallback
	);

	// Initialize the worker thread structure that exports the USDA file
	UsdaStageWriterWorker w;
	w.lastUpdateTime = &lastUpdateTime;
	w.lastUsdaWriteTime = &lastUsdaWriteTime;
	w.stage = stage;
	w.usdaPath = &usdaPath;

	// Create a running thread
	std::thread workerThread(&UsdaStageWriterWorker::doWork, &w);

	// Block here and exit when q or escape is pressed
	char c = 0;
	while (c != 'q' && c != 27)
	{
#ifdef _WIN32
		c = _getch();
#else
		c = getchar();
#endif
	}

	// Stop the thread
	w.stopped = true;

	// Wait for the thread to go away
	workerThread.join();

	// Cleanup callbacks
	omniClientStop(statSubscribeRequestId);
	pxr::TfNotice::Revoke(LayerReloadKey);
	pxr::TfNotice::Revoke(LayerChangeKey);
	pxr::TfNotice::Revoke(USDNoticeKey);

	// The stage is a sophisticated object that needs to be destroyed properly.  
	// Since stage is a smart pointer we can just reset it
	stage.Reset();

	omniClientShutdown();
}
