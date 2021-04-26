
std::vector<std::string> Splitter(const std::string& basetext, const char delim)
{
	std::string buf;				 // Have a buffer string
	std::stringstream ss(basetext);	 // Insert the string into a stream

	std::vector<std::string> tokens;  // Create vector to hold our words

	while (getline(ss, buf, delim))
		tokens.push_back(buf);
	return tokens;
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