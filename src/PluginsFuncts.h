
extern std::vector<std::string> Splitter(const std::string& basetext, char delim);
class myJSInstance
{
private:

public:
	myJSInstance();
	static bool RegisterFuncts(RE::BSScript::Internal::VirtualMachine* a_registry);
	static RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* GetGlobalFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	static RE::BSScript::ObjectTypeInfo::MemberFuncInfo* GetMemberFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	static std::vector<RE::BSScript::TypeInfo> GetFunctArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct);
	static std::vector<RE::BSScript::TypeInfo> GetFunctArgs(RE::BSScript::ObjectTypeInfo::MemberFuncInfo* globalFunct);
	static bool HandleSingleValue(std::vector<RE::BSScript::TypeInfo> argvals, RE::BSScript::TypeInfo typeValOne, std::string valStringOne, RE::BSScript::IFunctionArguments*& value1);
	static bool HandleTwoValues(std::vector<std::string> args, std::vector<RE::BSScript::TypeInfo> argvals, RE::BSScript::TypeInfo typeValOne, RE::BSScript::TypeInfo typeValtwo, std::string valStringOne, std::string valStringTwo, RE::BSScript::IFunctionArguments*& value1);
	static bool HandleThreeValues(std::vector<std::string> args, std::vector<RE::BSScript::TypeInfo> argvals, RE::BSScript::TypeInfo typeValOne, RE::BSScript::TypeInfo typeValtwo, RE::BSScript::TypeInfo typeValthree, std::string valStringOne, std::string valStringTwo, std::string valStringThree, RE::BSScript::IFunctionArguments*& value1);
	static RE::BSScript::IFunctionArguments* ConvertArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct, std::vector<std::string> args);
	static RE::BSScript::IFunctionArguments* ConvertArgs(RE::BSScript::ObjectTypeInfo::MemberFuncInfo* globalFunct, std::vector<std::string> args);
	static void CallGlobalFunction(RE::StaticFunctionTag*, RE::BSFixedString classfunct, RE::BSFixedString arglist);
	static void CallInstanceFunction(RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist);
	static std::vector<RE::BSScript::TypeInfo> getFunctArgsBody(const RE::BSTSmartPointer<RE::BSScript::IFunction>& innerFunct);
	static RE::BSScript::IFunctionArguments* getArgumentsBody(std::vector<std::string>& args, std::vector<RE::BSScript::TypeInfo>& argvals);
};