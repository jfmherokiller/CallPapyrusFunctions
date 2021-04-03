#include "quickjs.h"

extern void PrintToConsole(JSContext* ctx,JSValue val)
{
	const auto* const stats = JS_ToCString(ctx, val);
	if (std::strcmp(stats, "undefined") == 0)
		return;
	RE::ConsoleLog::GetSingleton()->Print("jseval:%s",stats);
	logger::info("jseval:%s", stats);
}
