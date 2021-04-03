extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	std::vector<spdlog::sink_ptr> sinks;
#ifndef NDEBUG

	sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());

#endif
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "MyFirstPlugin.log"sv;
	sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
	auto log = std::make_shared<spdlog::logger>("global log"s, begin(sinks), end(sinks));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info("MyFirstPlugin v1.0.0");

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "MyFirstPlugin";
	a_info->version = 1;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("MyFirstPlugin loaded");

	SKSE::Init(a_skse);
	logger::info("MyFirstPlugin loaded");
	auto papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus->Register(RegisterFuncs)) {
		return false;
	}
	return true;
}
