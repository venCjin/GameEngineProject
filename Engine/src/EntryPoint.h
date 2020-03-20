#pragma once

#include "Core\Core.h"
#include "Core\Assert.h"

#ifdef _WIN32
extern "C"
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;

	// https://community.amd.com/thread/169965
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
//else: Use GPU by default.
#endif

extern sixengine::Application* sixengine::CreateApplication();

int main(int argc, char** argv)
{
	sixengine::InitializeCore();

	sixengine::Application* app = sixengine::CreateApplication();
	CORE_ASSERT(app, "Client Application is null!");
	
	app->Run();
	
	delete app;
	sixengine::ShutdownCore();

	return 0;
}
