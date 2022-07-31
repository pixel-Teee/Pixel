#pragma once

//------my library------
#include "Pixel/Core/Core.h"
//------my library------

//------other library------
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
//ostr place front will cause error
//------other library------

namespace Pixel {
	class PIXEL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger(){ return s_CoreLogger;}
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger(){ return s_ClientLogger;}
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//core log macros
#define PIXEL_CORE_TRACE(...)	::Pixel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PIXEL_CORE_INFO(...)	::Pixel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PIXEL_CORE_ERROR(...)	::Pixel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PIXEL_CORE_WARN(...)	::Pixel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PIXEL_CORE_FATAL(...)	::Pixel::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//client log macros
#define PIXEL_TRACE(...)	::Pixel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PIXEL_INFO(...)		::Pixel::Log::GetClientLogger()->info(__VA_ARGS__)
#define PIXEL_ERROR(...)	::Pixel::Log::GetClientLogger()->error(__VA_ARGS__)
#define PIXEL_WARN(...)		::Pixel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PIXEL_FATAL(...)	::Pixel::Log::GetClientLogger()->fatal(__VA_ARGS__)

