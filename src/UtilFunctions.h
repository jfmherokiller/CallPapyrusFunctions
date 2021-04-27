//
// Created by peter on 4/27/21.
//

#ifndef CALLPAPARUSFUNCTIONS_UTILFUNCTIONS_H
#define CALLPAPARUSFUNCTIONS_UTILFUNCTIONS_H
extern std::vector<std::string> RemoveQuotesAndSplit(RE::BSFixedString input,char splitterchar);
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
#endif	//CALLPAPARUSFUNCTIONS_UTILFUNCTIONS_H
