#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace sixengine {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}
#ifdef DEBUG
// Core Logging Macros
#define LOG_CORE_TRACE(...)	sixengine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)	sixengine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)	sixengine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)	sixengine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...)	sixengine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Logging Macros
#define LOG_TRACE(...)	sixengine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)	sixengine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)	sixengine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)	sixengine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...)	sixengine::Log::GetClientLogger()->critical(__VA_ARGS__)
#else
// Core Logging Macros
#define LOG_CORE_TRACE(...)  
#define LOG_CORE_INFO(...)  
#define LOG_CORE_WARN(...)  
#define LOG_CORE_ERROR(...)  
#define LOG_CORE_FATAL(...)  

// Client Logging Macros
#define LOG_TRACE(...)  
#define LOG_INFO(...)  
#define LOG_WARN(...)  
#define LOG_ERROR(...)  
#define LOG_FATAL(...)  
#endif