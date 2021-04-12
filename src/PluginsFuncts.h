#include "quickjspp.hpp"
void PrintToConsole(JSContext* ctx,JSValue val);
extern int eval_buf(JSContext* ctx, const char* buf, size_t buf_len,const char* filename, int eval_flags);
extern void PrintStringToConsoleFile(const char* Part);
extern void PrintStringToConsoleFileFree(char* Part);
extern void PrintStringToConsoleFile2(const std::string& str);
class myJSInstance
{
private:

	/* make 'std' and 'os' visible to non module code */
	inline static const std::string exposeOSAndStd =
		"import * as std from 'std';\n"
		"import * as os from 'os';\n"
		"globalThis.std = std;\n"
		"globalThis.os = os;\n";

public:
	static qjs::Runtime* runtime;
	static qjs::Context* mycontext;
	myJSInstance();
	static void RemakeRuntime();
	static void ReinsertOSAndStd();
	static bool RegisterFuncts(RE::BSScript::Internal::VirtualMachine* a_registry);
	static void EvalateJsExpression(RE::StaticFunctionTag*, RE::BSFixedString expression);
	RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* GetGlobalFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	std::vector<RE::BSScript::TypeInfo> GetFunctArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct);
	RE::BSScript::IFunctionArguments* ConvertArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct, std::vector<std::string> args);
	void TestFunct(RE::StaticFunctionTag, RE::BSFixedString classfunct, RE::BSFixedString arglist);
	static void CustomModules();
};