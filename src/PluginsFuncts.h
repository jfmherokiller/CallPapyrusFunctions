#pragma once
class myJSInstance
{
private:

public:
	myJSInstance();
	static bool RegisterFuncts(RE::BSScript::Internal::VirtualMachine* a_registry);
	static RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* GetGlobalFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	static RE::BSScript::ObjectTypeInfo::MemberFuncInfo* GetMemberFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs);
	static void CallGlobalFunction([[maybe_unused]] [[maybe_unused]] RE::StaticFunctionTag*, RE::BSFixedString classfunct, RE::BSFixedString arglist);
	static void CallInstanceFunction([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist);
};