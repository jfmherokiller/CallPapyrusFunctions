#pragma once
#include <boost/algorithm/string.hpp>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "PluginsFuncts.h"
#include "UtilFunctions.h"
#include "NewTypeHandling.hpp"

#pragma warning(push)
#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

using namespace std::literals;

namespace logger = SKSE::log;

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
