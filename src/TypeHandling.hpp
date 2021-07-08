#pragma once
#define BsString(x) RE::BSFixedString(x)
#define StringToFloat(x) std::stof(x)
#define StringToInt(x) std::stoi(x)

class TypeHandling {
public:
    static bool HandleSingleValue(const RE::BSScript::TypeInfo& typeValOne, const std::string& valStringOne, RE::BSScript::IFunctionArguments*& value1);
    static bool HandleTwoValues(const RE::BSScript::TypeInfo& typeValOne, const RE::BSScript::TypeInfo& typeValtwo, const std::string& valStringOne, const std::string& valStringTwo, RE::BSScript::IFunctionArguments*& value1);
    static bool HandleThreeValues(const RE::BSScript::TypeInfo& typeValOne, const RE::BSScript::TypeInfo& typeValtwo, const RE::BSScript::TypeInfo& typeValthree, const std::string& valStringOne, const std::string& valStringTwo, const std::string& valStringThree, RE::BSScript::IFunctionArguments*& value1);
    static bool HandleObjectValue(const RE::BSScript::TypeInfo& typeValOne, const std::string& valStringOne, RE::BSScript::IFunctionArguments*& value1);
};