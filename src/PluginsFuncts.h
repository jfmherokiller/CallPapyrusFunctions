#include "quickjspp.hpp"
void PrintToConsole(JSContext* ctx,JSValue val);
extern int eval_buf(JSContext* ctx, const char* buf, size_t buf_len,const char* filename, int eval_flags);
extern bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_registry);
extern void PrintStringToConsoleFile(const char* Part);
extern void PrintStringToConsoleFileFree(char* Part);
class myJSInstance
{
private:
	qjs::Runtime runtime;
	qjs::Context mycontext;

public:
	myJSInstance();
	void RemakeRuntime();
};