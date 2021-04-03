#include "quickjs.h"

extern void PrintToConsole(JSContext* ctx,JSValue val)
{
	const auto* const stats = JS_ToCString(ctx, val);
	if (std::strcmp(stats, "undefined") == 0)
		return;
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s",stats);
	logger::info(FMT_STRING("jseval {}"), stats);
}
extern void PrintStringToConsoleFile(std::string Part)
{
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s", Part.c_str());
	logger::info(FMT_STRING("jseval {}"), Part.c_str());
}