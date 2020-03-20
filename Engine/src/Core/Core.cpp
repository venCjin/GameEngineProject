#include "pch.h"
#include "Core.h"
#include "Log.h"

#define ENGINE_BUILD_VERSION "v0.1"

namespace sixengine {

	void InitializeCore()
	{
		sixengine::Log::Init();

		LOG_CORE_TRACE("Six Engine {}", ENGINE_BUILD_VERSION);
		LOG_CORE_TRACE("Initializing...");
	}

	void ShutdownCore()
	{
		LOG_CORE_TRACE("Shutting down...");
	}

}
