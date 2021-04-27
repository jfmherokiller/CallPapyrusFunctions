//
// Created by peter on 4/27/21.
//

#ifndef CALLPAPARUSFUNCTIONS_UTILFUNCTIONS_H
#define CALLPAPARUSFUNCTIONS_UTILFUNCTIONS_H
extern std::vector<std::string> RemoveQuotesAndSplit(RE::BSFixedString input,char splitterchar);
template <typename T>
extern T* StringToForm(const std::string& formHex);
template <typename T>
extern RE::VMHandle StringToVmHandle(RE::BSScript::Internal::VirtualMachine* impvm,const std::string& formHex);
#endif	//CALLPAPARUSFUNCTIONS_UTILFUNCTIONS_H
