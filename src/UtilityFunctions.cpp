
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