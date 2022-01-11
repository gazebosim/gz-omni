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
# The "OmniUSDReader" application performs a few simple things:
#	* Expects one argument, the path to a USD stage
#		* Acceptable forms:
#			* omniverse://localhost/Users/test/helloworld.usd
#			* C:\USD\helloworld.usd
#			* A relative path based on the CWD of the program (helloworld.usd)
#	* Initialize Omniverse
#		* Set the Omniverse Client log callback (using a lambda)
#		* Set the Omniverse Client log level
#		* Initialize the Omniverse Client library
#		* Register an Omniverse Client status callback (using a static function)
#	* Open the USD stage
#	* Print the stage’s up-axis
#	* Print the stage’s linear units, or “meters per unit” setting
#	* Traverse the stage prims and print the path of each one
#	* Destroy the stage object
#	* Shutdown the Omniverse Client library
#
###############################################################################*/

#include <iostream>
#include <iomanip>
#include "OmniClient.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/metrics.h"

using namespace pxr;

static void OmniClientConnectionStatusCallbackImpl(void* userData, const char* url, OmniClientConnectionStatus status) noexcept
{
	std::cout << "Connection Status: " << omniClientGetConnectionStatusString(status) << " [" << url << "]" << std::endl;
	if (status == eOmniClientConnectionStatus_ConnectError)
	{
		// We shouldn't just exit here - we should clean up a bit, but we're going to do it anyway
		std::cout << "[ERROR] Failed connection, exiting." << std::endl;
		exit(-1);
	}
}

// Startup Omniverse 
static bool startOmniverse()
{
	// Register a function to be called whenever the library wants to print something to a log
	omniClientSetLogCallback(
		[](char const* threadName, char const* component, OmniClientLogLevel level, char const* message)
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

	omniClientRegisterConnectionStatusCallback(nullptr, OmniClientConnectionStatusCallbackImpl);

	return true;
}

// The program expects one argument, a path to a USD file
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Please provide an Omniverse stage URL to read." << std::endl;
        return -1;
    }

    std::cout << "Omniverse USD Stage Traversal: " << argv[1] << std::endl;
	
	startOmniverse();

	UsdStageRefPtr stage = UsdStage::Open(argv[1]);
	if (!stage)
	{
		std::cout << "Failure to open stage.  Exiting." << std::endl;
		return -2;
	}
	
	// Print the up-axis
	std::cout << "Stage up-axis: " << UsdGeomGetStageUpAxis(stage) << std::endl;

	// Print the stage's linear units, or "meters per unit"
	std::cout << "Meters per unit: " << std::setprecision(5) << UsdGeomGetStageMetersPerUnit(stage) << std::endl;

	// Traverse the stage and return the first UsdGeomMesh we find
	auto range = stage->Traverse();
	for (const auto& node : range)
	{
		std::cout << node.GetPath() << std::endl;
	}

	// The stage is a sophisticated object that needs to be destroyed properly.  
	// Since stage is a smart pointer we can just reset it
	stage.Reset();

	omniClientShutdown();
}
