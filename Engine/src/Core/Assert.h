#pragma once

#include "Log.h"

#ifdef DEBUG
	#define ENABLE_ASSERTS
#endif

#ifdef ENABLE_ASSERTS
	#define ASSERT(condition, ...) { if(!(condition)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CORE_ASSERT(condition, ...) { if(!(condition)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#else
	#define ASSERT(...)
	#define CORE_ASSERT(...)
#endif