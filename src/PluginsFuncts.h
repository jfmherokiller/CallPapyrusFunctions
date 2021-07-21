#pragma once
using BSScriptVmPtr = RE::BSScript::Internal::VirtualMachine*;
using globalFunctInfoPtr = RE::BSScript::ObjectTypeInfo::GlobalFuncInfo*;
using MemberFunctInfoPtr = RE::BSScript::ObjectTypeInfo::MemberFuncInfo*;
class myJSInstance
{
private:

public:
	myJSInstance();
	static bool RegisterFuncts(BSScriptVmPtr a_registry);
	static globalFunctInfoPtr GetGlobalFunction(BSScriptVmPtr impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	static MemberFunctInfoPtr GetMemberFunction(BSScriptVmPtr impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	static void CallGlobalFunction([[maybe_unused]] [[maybe_unused]] RE::StaticFunctionTag*, RE::BSFixedString classfunct, RE::BSFixedString arglist);
	static void CallInstanceFunction([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString Instance, RE::BSFixedString classfunct, RE::BSFixedString arglist);
};
