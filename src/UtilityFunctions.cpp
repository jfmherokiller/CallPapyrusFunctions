﻿

extern void PrintToConsole(JSContext* ctx,JSValue val)
{
	const auto* const stats = JS_ToCString(ctx, val);
	if (std::strcmp(stats, "undefined") == 0)
	{
		JS_FreeCString(ctx, stats);
		return;
	}
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s",stats);
	logger::info(FMT_STRING("jseval:{}"), stats);
	JS_FreeCString(ctx, stats);
}
extern void PrintStringToConsoleFile(const char* Part)
{
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s", Part);
	logger::info(FMT_STRING("jseval:{}"), Part);
}
extern void PrintStringToConsoleFile2(const std::string& str)
{
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s", str.c_str());
	logger::info(FMT_STRING("jseval:{}"), str.c_str());
}
extern void PrintStringToConsoleFileFree(char* Part)
{
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s", Part);
	logger::info(FMT_STRING("jseval:{}"), Part);
	free(Part);
}