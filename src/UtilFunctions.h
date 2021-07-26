//
// Created by peter on 4/27/21.
//

#pragma once
extern std::vector<std::string> RemoveQuotesAndSplit(const RE::BSFixedString& input,char splitterchar);
extern std::string RemoveDoubleQuoteChars(const RE::BSFixedString& input);

template <typename T>
T* StringToForm(const std::string& formHex)
{
	const RE::FormID Playerform = std::strtoul(formHex.c_str(), nullptr, 16);
	const auto RefTesting2 = RE::TESForm::LookupByID<T>(Playerform);
	return RefTesting2;
}
template <typename T>
RE::VMHandle StringToVmHandle(RE::BSScript::Internal::VirtualMachine* impvm,const std::string& formHex)
{
	auto policy = impvm->GetObjectHandlePolicy();
	const RE::FormID Playerform = std::strtoul(formHex.c_str(), nullptr, 16);
	const auto RefTesting2 = RE::TESForm::LookupByID<T>(Playerform);
	const auto ObjectVmHandle = policy->GetHandleForObject(RefTesting2->GetFormType(), RefTesting2);
	return ObjectVmHandle;
}
void StringToObject(RE::BSScript::Internal::VirtualMachine* impvm, const std::string& formHex, const RE::BSScript::TypeInfo& TypeArg, RE::BSTSmartPointer<RE::BSScript::Object>& myObject);
inline const RE::BSScript::ObjectTypeInfo* getDesiredTypeInfoFromScripts(const std::string& ClassNamePart, const RE::BSTSmallSharedArray<RE::BSScript::Internal::AttachedScript>* mattached_scripts)
{
    RE::BSScript::ObjectTypeInfo* FoundType = nullptr;
    for (const auto& script : *mattached_scripts) {
        auto ScriptName = boost::algorithm::to_lower_copy(std::string(script->GetTypeInfo()->GetName()));
        if ((ClassNamePart == ScriptName) || (ScriptName.starts_with(ClassNamePart))) {
            FoundType = script->GetTypeInfo();
        }
    }
    return FoundType;
}
inline const RE::BSTSmallSharedArray<RE::BSScript::Internal::AttachedScript>* GetAttachedScriptsFromVmHandle(RE::BSScript::Internal::VirtualMachine* impvm, RE::VMHandle ObjectVmHandle)
{
    const RE::BSTSmallSharedArray<RE::BSScript::Internal::AttachedScript>* FoundType = nullptr;
    for (const auto& item : impvm->attachedScripts) {
        if(item.first == ObjectVmHandle) {
            FoundType = &item.second;
        }
    }
    return FoundType;
}
RE::VMTypeID StringToVmType(const std::string& TypeName);

template <class F>
void call_async(F&& fun) {
    auto futptr = std::make_shared<std::future<void>>();
    *futptr = std::async(std::launch::async, [futptr, fun]() {
      fun();
    });
}