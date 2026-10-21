#pragma once

#define DEBUG

#define NOMINMAX

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <xbyak/xbyak.h>

#pragma warning(push)
#ifdef DEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

using namespace std::literals;

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
